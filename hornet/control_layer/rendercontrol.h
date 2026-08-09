#pragma once
#include <QHash>
#include <QObject>
#include <QSet>
#include <filesystem>
class GridService;
class QTimer;
class RenderControl : public QObject
{
    Q_OBJECT
public:
    explicit RenderControl(GridService &gridService);

    // ================================================================
    // SLICE: hornet render / hornet trust entry points
    // ================================================================
    QString dispatchRenderCommand(int boxId, const std::filesystem::path &workingDir);
    QString dispatchTrustCommand(int boxId,
                                 const QString &sourceName,
                                 const std::filesystem::path &workingDir);
signals:
    void sourceValueUpdated();

private:
    // ================================================================
    // SLICE: async fetch + auto-repeat (once trusted and fetched once)
    // ================================================================
    void fetchSource(int boxId,
                     const QString &sourceName,
                     const QString &command,
                     const std::filesystem::path &workingDir);
    void startAutoRepeat(int boxId,
                         const QString &sourceName,
                         const QString &command,
                         const std::filesystem::path &workingDir);
    static QString makeKey(int boxId, const QString &sourceName);

    GridService &m_gridService;
    QSet<QString> m_trustedCommands; // exact command strings, session-only
    QSet<QString> m_inFlightKeys;    // "boxId:name" - guards overlapping fetches per source
    QHash<QString, QTimer *> m_autoRepeatTimers; // "boxId:name" -> timer
    static constexpr int m_autoRepeatIntervalMs = 1000;
};
