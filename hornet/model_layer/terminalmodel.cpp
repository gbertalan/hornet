#include "terminalmodel.h"

const std::filesystem::path &TerminalModel::getCurrentDirectory() const
{
    return m_currentDirectory;
}

void TerminalModel::setCurrentDirectory(const std::filesystem::path &path)
{
    m_currentDirectory = path;
}

const std::vector<TerminalPromptAndDir> &TerminalModel::getTerminalPromptAndDirs() const
{
    return m_terminalPromptAndDirs;
}

void TerminalModel::addTerminalPromptAndDir(const TerminalPromptAndDir &terminalPromptAndDir)
{
    m_terminalPromptAndDirs.push_back(terminalPromptAndDir);
}

void TerminalModel::updateLineDirectory(int index,
                                        const std::filesystem::path &directory,
                                        const std::u32string &prompt)
{
    if (index >= 0 && index < static_cast<int>(m_terminalPromptAndDirs.size())) {
        m_terminalPromptAndDirs.at(index).directory = directory;
        m_terminalPromptAndDirs.at(index).prompt = prompt;
    }
}

void TerminalModel::removeTerminalPromptAndDir(int index)
{
    if (index >= 0 && index < static_cast<int>(m_terminalPromptAndDirs.size()))
        m_terminalPromptAndDirs.erase(m_terminalPromptAndDirs.begin() + index);
}
