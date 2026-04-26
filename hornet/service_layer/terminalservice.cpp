#include "terminalservice.h"
#include "editorservice.h"
#include <iostream>

TerminalService::TerminalService(EditorService &editorService)
    : m_editorService(editorService)
{}

void TerminalService::addTerminalPromptAndDir(const TerminalPromptAndDir &promptAndDir)
{
    m_terminalModel.addTerminalPromptAndDir(promptAndDir);
}

const std::vector<TerminalPromptAndDir> &TerminalService::getTerminalPromptAndDirs() const
{
    return m_terminalModel.getTerminalTerminalPromptAndDirs();
}

void TerminalService::initialize()
{
    m_editorService.setTextLines({U""}, "txt");
    m_terminalModel.addTerminalPromptAndDir(
        {getCurrentPrompt(), m_terminalModel.getCurrentDirectory()});
}

std::u32string TerminalService::getCurrentPrompt() const
{
    std::filesystem::path dir = m_terminalModel.getCurrentDirectory();
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
    return m_terminalModel.getCurrentDirectory();
}

void TerminalService::setCurrentDirectory(const std::filesystem::path &path)
{
    m_terminalModel.setCurrentDirectory(path);
}

void TerminalService::updateTerminalLineDirectory(int index, const std::filesystem::path &directory)
{
    m_terminalModel.updateLineDirectory(index, directory, getCurrentPrompt());
}
