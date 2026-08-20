#include "gdbcontrol.h"
#include <QDebug>
#include <QRegularExpression>
#include <fcntl.h>
#include <unistd.h>

GdbControl::GdbControl(QObject *parent)
    : QObject(parent)
{
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &GdbControl::onReadyRead);
    connect(&m_process, &QProcess::finished, this, &GdbControl::onProcessFinished);
}

QString GdbControl::dispatchStart(const QString &binaryPath, const std::filesystem::path &workingDir)
{
    if (m_sessionActive)
        return "gdb session already active - run 'hornet gdb stop' first";

    m_process.setWorkingDirectory(QString::fromStdString(workingDir.string()));
    m_process.start("gdb", QStringList() << "--interpreter=mi4" << "-nx" << "-q" << binaryPath);
    if (!m_process.waitForStarted(2000))
        return "could not start gdb (is it installed and on PATH?)";

    m_sessionActive = true;
    if (setupInferiorPty()) {
        const int token = m_nextToken++;
        m_pendingCommandText.insert(token, "-inferior-tty-set");
        m_process.write(QString::number(token).toUtf8() + "-inferior-tty-set "
                        + m_inferiorPtyPath.toUtf8() + "\n");
    }
    return "gdb session started for " + binaryPath;
}

QString GdbControl::dispatchStop()
{
    if (!m_sessionActive)
        return "no active gdb session";
    m_process.write("-gdb-exit\n");
    return "";
}

QString GdbControl::dispatchRaw(const QString &miCommand)
{
    if (!m_sessionActive)
        return "no active gdb session - run 'hornet gdb start <binary>' first";

    const int token = m_nextToken++;
    m_pendingCommandText.insert(token, miCommand);
    m_process.write(QString::number(token).toUtf8() + miCommand.toUtf8() + "\n");
    return "";
}

void GdbControl::dispatchSourceQuery(int boxId, const QString &sourceName, const QString &miCommand)
{
    if (!m_sessionActive)
        return; // no session - source stays unresolved, same as "not fetched" in the parser

    const int token = m_nextToken++;
    m_pendingSourceQueries.insert(token, {boxId, sourceName});
    m_process.write(QString::number(token).toUtf8() + miCommand.toUtf8() + "\n");
}

QString GdbControl::dispatchRawDebugPrint(const QString &miCommand)
{
    if (!m_sessionActive)
        return "no active gdb session - run 'hornet gdb start <binary>' first";

    const int token = m_nextToken++;
    m_pendingDebugPrintCommands.insert(token, miCommand);
    m_process.write(QString::number(token).toUtf8() + miCommand.toUtf8() + "\n");
    return "";
}

static QString extractMiValueField(const QString &resultText)
{
    static const QRegularExpression valuePattern("value=\"((?:[^\"\\\\]|\\\\.)*)\"");
    const QRegularExpressionMatch match = valuePattern.match(resultText);
    if (!match.hasMatch())
        return QString();
    QString value = match.captured(1);
    value.replace("\\\"", "\"").replace("\\\\", "\\");
    return value;
}

void GdbControl::onReadyRead()
{
    m_lineBuffer.append(m_process.readAllStandardOutput());
    int newlineIndex;
    while ((newlineIndex = m_lineBuffer.indexOf('\n')) != -1) {
        const QByteArray lineBytes = m_lineBuffer.left(newlineIndex);
        m_lineBuffer.remove(0, newlineIndex + 1);
        processLine(QString::fromUtf8(lineBytes).trimmed());
    }
}

void GdbControl::processLine(const QString &line)
{
    if (line.isEmpty() || line == "(gdb)")
        return;

    static const QRegularExpression resultPattern("^(\\d+)\\^(done|error|running|exit)(.*)$");
    const QRegularExpressionMatch match = resultPattern.match(line);
    if (match.hasMatch()) {
        const int token = match.captured(1).toInt();
        const QString resultText = match.captured(2) + match.captured(3);

        if (m_pendingDebugPrintCommands.contains(token)) {
            const QString commandText = m_pendingDebugPrintCommands.take(token);
            qDebug().noquote() << "[gdb]" << commandText << "->" << resultText;
            return;
        }

        if (m_pendingSourceQueries.contains(token)) {
            const PendingSourceQuery pending = m_pendingSourceQueries.take(token);
            const QString value = extractMiValueField(resultText);
            if (!value.isNull())
                emit sourceQueryCompleted(pending.boxId, pending.sourceName, value);
            // else: no "value=" field (e.g. process not running), keep showing last value
            return;
        }

        const QString commandText = m_pendingCommandText.take(token);
        emit commandCompleted(commandText, resultText);
        return;
    }

    emit asyncNotificationReceived(line);
}

#include <fcntl.h>
#include <unistd.h>

bool GdbControl::setupInferiorPty()
{
    const int masterFd = posix_openpt(O_RDWR | O_NOCTTY);
    if (masterFd == -1)
        return false;
    if (grantpt(masterFd) != 0 || unlockpt(masterFd) != 0) {
        ::close(masterFd);
        return false;
    }
    const char *slaveName = ptsname(masterFd);
    if (!slaveName) {
        ::close(masterFd);
        return false;
    }

    m_inferiorPtyMasterFd = masterFd;
    m_inferiorPtyPath = QString::fromLocal8Bit(slaveName);
    m_inferiorPtyNotifier = new QSocketNotifier(masterFd, QSocketNotifier::Read, this);
    connect(m_inferiorPtyNotifier,
            &QSocketNotifier::activated,
            this,
            &GdbControl::onInferiorPtyReadyRead);
    return true;
}

void GdbControl::onInferiorPtyReadyRead()
{
    char buffer[4096];
    const ssize_t bytesRead = ::read(m_inferiorPtyMasterFd, buffer, sizeof(buffer));
    if (bytesRead > 0)
        emit targetOutputReceived(QString::fromUtf8(buffer, static_cast<int>(bytesRead)));
}

void GdbControl::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_inferiorPtyNotifier) {
        m_inferiorPtyNotifier->setEnabled(false);
        m_inferiorPtyNotifier->deleteLater();
        m_inferiorPtyNotifier = nullptr;
    }
    if (m_inferiorPtyMasterFd != -1) {
        ::close(m_inferiorPtyMasterFd);
        m_inferiorPtyMasterFd = -1;
    }

    const bool wasIntentional = m_sessionActive;
    m_sessionActive = false;
    m_pendingCommandText.clear();
    m_pendingSourceQueries.clear();
    m_lineBuffer.clear();

    const QString reason = exitStatus == QProcess::CrashExit
                               ? "gdb crashed"
                               : "gdb session ended (exit code " + QString::number(exitCode) + ")";
    emit sessionEnded(wasIntentional ? reason : reason + " (unexpected)");
}