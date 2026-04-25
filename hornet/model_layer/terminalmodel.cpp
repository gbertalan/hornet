#include "terminalmodel.h"

const std::vector<std::u32string> &TerminalModel::getCommandHistory() const
{
    return m_commandHistory;
}

void TerminalModel::addToHistory(const std::u32string &command)
{
    m_commandHistory.push_back(command);
}

int TerminalModel::getHistoryIndex() const
{
    return m_historyIndex;
}

void TerminalModel::setHistoryIndex(int index)
{
    m_historyIndex = index;
}

const std::filesystem::path &TerminalModel::getCurrentDirectory() const
{
    return m_currentDirectory;
}

void TerminalModel::setCurrentDirectory(const std::filesystem::path &path)
{
    m_currentDirectory = path;
}
