#include "terminalservice.h"
#include "editorservice.h"
#include "model_layer/imodelaccess_readwrite.h"

TerminalService::TerminalService(IModelAccessReadWrite &modelAccess, EditorService &editorService)
    : m_modelAccess(modelAccess)
    , m_editorService(editorService)
{}

void TerminalService::init()
{
    m_modelAccess.getTerminalModel().addTerminalPromptAndDir(
        {retrieveCurrentPrompt(), m_modelAccess.getTerminalModel().retrieveCurrentDirectory()});
}

void TerminalService::addTerminalPromptAndDir(const TerminalPromptAndDir &promptAndDir)
{
    m_modelAccess.getTerminalModel().addTerminalPromptAndDir(promptAndDir);
}

const std::vector<TerminalPromptAndDir> &TerminalService::retrieveTerminalPromptAndDirs() const
{
    return m_modelAccess.getTerminalModel().retrieveTerminalPromptAndDirs();
}

std::u32string TerminalService::retrieveCurrentPrompt() const
{
    std::filesystem::path dir = m_modelAccess.getTerminalModel().retrieveCurrentDirectory();
    std::string result;
    std::filesystem::path homeDir;
#ifdef _WIN32
    homeDir = std::filesystem::path(std::getenv("USERPROFILE"));
#else
    homeDir = std::filesystem::path(std::getenv("HOME"));
#endif
    if (dir == homeDir)
        result = "~/";
    else if (dir == dir.root_path())
        result = "/";
    else
        result = "../" + dir.filename().string() + "/";
    return std::u32string(result.begin(), result.end());
}

const std::filesystem::path &TerminalService::retrieveCurrentDirectory() const
{
    return m_modelAccess.getTerminalModel().retrieveCurrentDirectory();
}

void TerminalService::storeCurrentDirectory(const std::filesystem::path &path)
{
    m_modelAccess.getTerminalModel().storeCurrentDirectory(path);
}

void TerminalService::storeTerminalLineDirectory(int index, const std::filesystem::path &directory)
{
    m_modelAccess.getTerminalModel().updateLineDirectory(index, directory, retrieveCurrentPrompt());
}

void TerminalService::removeTerminalPromptAndDir(int index)
{
    m_modelAccess.getTerminalModel().removeTerminalPromptAndDir(index);
}
