#pragma once
#include <QHash>
#include <QObject>
#include <QSet>
#include <QStringList>
#include "shared/dto_model_to_view/toollistsourcedto.h"
#include "shared/dto_model_to_view/toolsourcedto.h"
#include <filesystem>
class GridService;
class GdbControl;
class QTimer;
class ToolControl : public QObject
{
    Q_OBJECT
public:
    explicit ToolControl(GridService &gridService, GdbControl &gdbControl);
    QString dispatchToolCommand(int boxId, const std::filesystem::path &workingDir);
    QString dispatchTrustCommand(int boxId,
                                 const QString &sourceName,
                                 const std::filesystem::path &workingDir);
    bool isCommandTrusted(const QString &command) const;
    void trustCommands(const QStringList &commands);
signals:
    void sourceValueUpdated();

private:
    void performFetch(int boxId,
                      const QString &sourceName,
                      const QString &command,
                      const std::filesystem::path &workingDir,
                      int intervalMs);
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
                      const ToolSourceDTO &source,
                      const std::filesystem::path &workingDir);

    void attemptListFetch(int boxId,
                          const ToolListSourceDTO &listSource,
                          const std::filesystem::path &workingDir);
    void fetchListSource(int boxId,
                         const QString &name,
                         const QString &command,
                         const std::filesystem::path &workingDir,
                         int intervalMs);
    void startListAutoRepeat(int boxId,
                             const QString &name,
                             const QString &command,
                             const std::filesystem::path &workingDir,
                             int intervalMs);

    GridService &m_gridService;
    GdbControl &m_gdbControl;
    QSet<QString> m_trustedCommands;
    QSet<QString> m_inFlightKeys;
    QHash<QString, QTimer *> m_autoRepeatTimers;
    QSet<QString> m_listInFlightKeys;
    QHash<QString, QTimer *> m_listAutoRepeatTimers;
};