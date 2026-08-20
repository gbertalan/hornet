#include "toolcontrol.h"
#include <QProcess>
#include <QTimer>
#include "control_layer/gdbcontrol.h"
#include "service_layer/gridservice.h"
#include "shared/dto_model_to_view/toollistsourcedto.h"
#include "shared/dto_model_to_view/toolsourcedto.h"

ToolControl::ToolControl(GridService &gridService, GdbControl &gdbControl)
    : m_gridService(gridService)
    , m_gdbControl(gdbControl)
{
    connect(&m_gdbControl,
            &GdbControl::sourceQueryCompleted,
            this,
            [this](int boxId, const QString &sourceName, const QString &value) {
                m_gridService.storeToolSourceValue(boxId, sourceName, value);
                emit sourceValueUpdated();
            });
}

QString ToolControl::makeKey(int boxId, const QString &sourceName)
{
    return QString::number(boxId) + ":" + sourceName;
}

void ToolControl::attemptFetch(int boxId,
                               const ToolSourceDTO &source,
                               const std::filesystem::path &workingDir)
{
    const QString key = makeKey(boxId, source.name);
    if (source.intervalMs > 0 && m_autoRepeatTimers.contains(key))
        return;
    performFetch(boxId, source.name, source.command, workingDir, source.intervalMs);
}

QString ToolControl::dispatchToolCommand(int boxId, const std::filesystem::path &workingDir)
{
    const std::vector<ToolSourceDTO> sources = m_gridService.retrieveToolSources(boxId);
    const std::vector<ToolListSourceDTO> listSources = m_gridService.retrieveToolListSources(boxId);
    if (sources.empty() && listSources.empty())
        return "no data sources declared in this box";

    QStringList untrusted;
    for (const ToolSourceDTO &source : sources) {
        if (!m_trustedCommands.contains(source.command)) {
            untrusted.push_back(source.name);
            continue;
        }
        attemptFetch(boxId, source, workingDir);
    }
    for (const ToolListSourceDTO &listSource : listSources) {
        if (!m_trustedCommands.contains(listSource.command)) {
            untrusted.push_back(listSource.name);
            continue;
        }
        attemptListFetch(boxId, listSource, workingDir);
    }

    if (!untrusted.isEmpty())
        return "not yet trusted, run 'hornet trust " + QString::number(boxId)
               + " <name>' for: " + untrusted.join(", ");
    return "";
}

QString ToolControl::dispatchTrustCommand(int boxId,
                                          const QString &sourceName,
                                          const std::filesystem::path &workingDir)
{
    const std::vector<ToolSourceDTO> sources = m_gridService.retrieveToolSources(boxId);
    for (const ToolSourceDTO &source : sources) {
        if (source.name == sourceName) {
            m_trustedCommands.insert(source.command);
            attemptFetch(boxId, source, workingDir);
            return "trusted and fetching '" + sourceName + "'";
        }
    }
    const std::vector<ToolListSourceDTO> listSources = m_gridService.retrieveToolListSources(boxId);
    for (const ToolListSourceDTO &listSource : listSources) {
        if (listSource.name == sourceName) {
            m_trustedCommands.insert(listSource.command);
            attemptListFetch(boxId, listSource, workingDir);
            return "trusted and fetching '" + sourceName + "'";
        }
    }
    return "no data source named '" + sourceName + "' in this box";
}

void ToolControl::attemptListFetch(int boxId,
                                   const ToolListSourceDTO &listSource,
                                   const std::filesystem::path &workingDir)
{
    const QString key = makeKey(boxId, listSource.name);
    if (listSource.intervalMs > 0 && m_listAutoRepeatTimers.contains(key))
        return;
    fetchListSource(boxId, listSource.name, listSource.command, workingDir, listSource.intervalMs);
}

void ToolControl::fetchListSource(int boxId,
                                  const QString &name,
                                  const QString &command,
                                  const std::filesystem::path &workingDir,
                                  int intervalMs)
{
    static const QString gdbPrefix = "gdb ";
    if (command.startsWith(gdbPrefix))
        return; // gdb-backed list sources land in a later stage - not wired yet

    const QString key = makeKey(boxId, name);
    if (m_listInFlightKeys.contains(key))
        return;
    m_listInFlightKeys.insert(key);

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(QString::fromStdString(workingDir.string()));
    connect(process,
            &QProcess::finished,
            this,
            [this, process, boxId, name, command, workingDir, key, intervalMs]() {
                const QString output = QString::fromUtf8(process->readAllStandardOutput()).trimmed();
                QVector<QString> rows;
                for (const QString &line : output.split('\n'))
                    rows.push_back(line);
                m_gridService.upsertListBox(name, rows);
                m_listInFlightKeys.remove(key);
                process->deleteLater();
                emit sourceValueUpdated();

                if (intervalMs > 0 && !m_listAutoRepeatTimers.contains(key))
                    startListAutoRepeat(boxId, name, command, workingDir, intervalMs);
            });
    process->start("/bin/sh", QStringList() << "-c" << command);
}

void ToolControl::startListAutoRepeat(int boxId,
                                      const QString &name,
                                      const QString &command,
                                      const std::filesystem::path &workingDir,
                                      int intervalMs)
{
    const QString key = makeKey(boxId, name);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, boxId, name, command, workingDir, intervalMs]() {
        fetchListSource(boxId, name, command, workingDir, intervalMs);
    });
    timer->start(intervalMs);
    m_listAutoRepeatTimers.insert(key, timer);
}

bool ToolControl::isCommandTrusted(const QString &command) const
{
    return m_trustedCommands.contains(command);
}

void ToolControl::trustCommands(const QStringList &commands)
{
    for (const QString &command : commands)
        m_trustedCommands.insert(command);
}

void ToolControl::performFetch(int boxId,
                               const QString &sourceName,
                               const QString &command,
                               const std::filesystem::path &workingDir,
                               int intervalMs)
{
    static const QString gdbPrefix = "gdb ";
    if (command.startsWith(gdbPrefix)) {
        m_gdbControl.dispatchSourceQuery(boxId, sourceName, command.mid(gdbPrefix.length()));
        const QString key = makeKey(boxId, sourceName);
        if (intervalMs > 0 && !m_autoRepeatTimers.contains(key))
            startAutoRepeat(boxId, sourceName, command, workingDir, intervalMs);
        return;
    }
    fetchSource(boxId, sourceName, command, workingDir, intervalMs);
}

void ToolControl::fetchSource(int boxId,
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
                m_gridService.storeToolSourceValue(boxId, sourceName, output);
                m_inFlightKeys.remove(key);
                process->deleteLater();
                emit sourceValueUpdated();

                if (intervalMs > 0 && !m_autoRepeatTimers.contains(key))
                    startAutoRepeat(boxId, sourceName, command, workingDir, intervalMs);
            });
    process->start("/bin/sh", QStringList() << "-c" << command);
}

void ToolControl::startAutoRepeat(int boxId,
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
                performFetch(boxId, sourceName, command, workingDir, intervalMs);
            });
    timer->start(intervalMs);
    m_autoRepeatTimers.insert(key, timer);
}