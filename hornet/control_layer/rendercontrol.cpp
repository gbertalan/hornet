#include "rendercontrol.h"
#include <QProcess>
#include <QTimer>
#include "service_layer/gridservice.h"
#include "shared/dto_model_to_view/rendersourcedto.h"

RenderControl::RenderControl(GridService &gridService)
    : m_gridService(gridService)
{}

QString RenderControl::makeKey(int boxId, const QString &sourceName)
{
    return QString::number(boxId) + ":" + sourceName;
}

// ================================================================
// SLICE: hornet render / hornet trust entry points
// ================================================================

void RenderControl::attemptFetch(int boxId,
                                 const RenderSourceDTO &source,
                                 const std::filesystem::path &workingDir)
{
    const QString key = makeKey(boxId, source.name);
    if (source.intervalMs > 0 && m_autoRepeatTimers.contains(key))
        return; // already auto-repeating - no need to trigger an extra fetch
    fetchSource(boxId, source.name, source.command, workingDir, source.intervalMs);
}

QString RenderControl::dispatchRenderCommand(int boxId, const std::filesystem::path &workingDir)
{
    const std::vector<RenderSourceDTO> sources = m_gridService.retrieveRenderSources(boxId);
    if (sources.empty())
        return "no data sources declared in this box";

    QStringList untrusted;
    for (const RenderSourceDTO &source : sources) {
        if (!m_trustedCommands.contains(source.command)) {
            untrusted.push_back(source.name);
            continue;
        }
        attemptFetch(boxId, source, workingDir);
    }

    if (!untrusted.isEmpty())
        return "not yet trusted, run 'hornet trust " + QString::number(boxId)
               + " <name>' for: " + untrusted.join(", ");
    return "";
}

QString RenderControl::dispatchTrustCommand(int boxId,
                                            const QString &sourceName,
                                            const std::filesystem::path &workingDir)
{
    const std::vector<RenderSourceDTO> sources = m_gridService.retrieveRenderSources(boxId);
    for (const RenderSourceDTO &source : sources) {
        if (source.name == sourceName) {
            m_trustedCommands.insert(source.command);
            attemptFetch(boxId, source, workingDir);
            return "trusted and fetching '" + sourceName + "'";
        }
    }
    return "no data source named '" + sourceName + "' in this box";
}

void RenderControl::fetchSource(int boxId,
                                const QString &sourceName,
                                const QString &command,
                                const std::filesystem::path &workingDir,
                                int intervalMs)
{
    const QString key = makeKey(boxId, sourceName);
    if (m_inFlightKeys.contains(key))
        return;
    m_inFlightKeys.insert(key);

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(QString::fromStdString(workingDir.string()));
    connect(process,
            &QProcess::finished,
            this,
            [this, process, boxId, sourceName, command, workingDir, key, intervalMs]() {
                const QString output = QString::fromUtf8(process->readAllStandardOutput()).trimmed();
                m_gridService.storeRenderSourceValue(boxId, sourceName, output);
                m_inFlightKeys.remove(key);
                process->deleteLater();
                emit sourceValueUpdated();

                if (intervalMs > 0 && !m_autoRepeatTimers.contains(key))
                    startAutoRepeat(boxId, sourceName, command, workingDir, intervalMs);
            });
    process->start("/bin/sh", QStringList() << "-c" << command);
}

void RenderControl::startAutoRepeat(int boxId,
                                    const QString &sourceName,
                                    const QString &command,
                                    const std::filesystem::path &workingDir,
                                    int intervalMs)
{
    const QString key = makeKey(boxId, sourceName);
    QTimer *timer = new QTimer(this);
    connect(timer,
            &QTimer::timeout,
            this,
            [this, boxId, sourceName, command, workingDir, intervalMs]() {
                fetchSource(boxId, sourceName, command, workingDir, intervalMs);
            });
    timer->start(intervalMs);
    m_autoRepeatTimers.insert(key, timer);
}
