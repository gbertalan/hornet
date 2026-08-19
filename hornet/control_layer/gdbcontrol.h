#pragma once
#include <QHash>
#include <QObject>
#include <QProcess>
#include <filesystem>

class GdbControl : public QObject
{
    Q_OBJECT
public:
    explicit GdbControl(QObject *parent = nullptr);

    QString dispatchStart(const QString &binaryPath, const std::filesystem::path &workingDir);
    QString dispatchStop();
    QString dispatchRaw(const QString &miCommand);
    void dispatchSourceQuery(int boxId, const QString &sourceName, const QString &miCommand);

signals:
    void commandCompleted(const QString &commandText, const QString &resultText);
    void asyncNotificationReceived(const QString &line);
    void sessionEnded(const QString &reason);
    void sourceQueryCompleted(int boxId, const QString &sourceName, const QString &value);

private slots:
    void onReadyRead();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void processLine(const QString &line);

    struct PendingSourceQuery
    {
        int boxId;
        QString sourceName;
    };

    QProcess m_process;
    bool m_sessionActive = false;
    int m_nextToken = 1;
    QHash<int, QString> m_pendingCommandText;
    QHash<int, PendingSourceQuery> m_pendingSourceQueries;
    QByteArray m_lineBuffer;
};