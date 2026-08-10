#pragma once
#include <QHash>
#include <QObject>
#include <QSet>
#include "shared/dto_model_to_view/rendersourcedto.h"
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
                     const std::filesystem::path &workingDir,
                     int intervalMs);
    void startAutoRepeat(int boxId,
                         const QString &sourceName,
                         const QString &command,
                         const std::filesystem::path &workingDir,
                         int intervalMs);
    static QString makeKey(int boxId, const QString &sourceName);
    void attemptFetch(int boxId,
                      const RenderSourceDTO &source,
                      const std::filesystem::path &workingDir);

    GridService &m_gridService;
    QSet<QString> m_trustedCommands; // exact command strings, session-only
    QSet<QString> m_inFlightKeys;    // "boxId:name" - guards overlapping fetches per source
    QHash<QString, QTimer *> m_autoRepeatTimers; // "boxId:name" -> timer
};
