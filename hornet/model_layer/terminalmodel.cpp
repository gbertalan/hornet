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

const std::vector<TerminalLine> &TerminalModel::getTerminalLines() const
{
    return m_terminalLines;
}

void TerminalModel::addTerminalLine(const TerminalLine &line)
{
    m_terminalLines.push_back(line);
}

void TerminalModel::updateLineDirectory(int index,
                                        const std::filesystem::path &directory,
                                        const std::u32string &prompt)
{
    if (index >= 0 && index < static_cast<int>(m_terminalLines.size())) {
        m_terminalLines.at(index).directory = directory;
        m_terminalLines.at(index).prompt = prompt;
    }
}
