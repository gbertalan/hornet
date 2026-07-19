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
        {getCurrentPrompt(), m_modelAccess.getTerminalModel().getCurrentDirectory()});
}

void TerminalService::addTerminalPromptAndDir(const TerminalPromptAndDir &promptAndDir)
{
    m_modelAccess.getTerminalModel().addTerminalPromptAndDir(promptAndDir);
}

const std::vector<TerminalPromptAndDir> &TerminalService::getTerminalPromptAndDirs() const
{
    return m_modelAccess.getTerminalModel().getTerminalPromptAndDirs();
}

std::u32string TerminalService::getCurrentPrompt() const
{
    std::filesystem::path dir = m_modelAccess.getTerminalModel().getCurrentDirectory();
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

const std::filesystem::path &TerminalService::getCurrentDirectory() const
{
    return m_modelAccess.getTerminalModel().getCurrentDirectory();
}

void TerminalService::setCurrentDirectory(const std::filesystem::path &path)
{
    m_modelAccess.getTerminalModel().setCurrentDirectory(path);
}

void TerminalService::updateTerminalLineDirectory(int index, const std::filesystem::path &directory)
{
    m_modelAccess.getTerminalModel().updateLineDirectory(index, directory, getCurrentPrompt());
}

void TerminalService::removeTerminalPromptAndDir(int index)
{
    m_modelAccess.getTerminalModel().removeTerminalPromptAndDir(index);
}
