#include "gdbcontrol.h"
#include <QRegularExpression>

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

        if (m_pendingSourceQueries.contains(token)) {
            const PendingSourceQuery pending = m_pendingSourceQueries.take(token);
            emit sourceQueryCompleted(pending.boxId,
                                      pending.sourceName,
                                      extractMiValueField(resultText));
            return;
        }

        const QString commandText = m_pendingCommandText.take(token);
        emit commandCompleted(commandText, resultText);
        return;
    }

    emit asyncNotificationReceived(line);
}

void GdbControl::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
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