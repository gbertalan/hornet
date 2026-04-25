#pragma once

#include <filesystem>
#include <string>
#include <vector>

class TerminalModel
{
public:
    const std::vector<std::u32string> &getCommandHistory() const;
    void addToHistory(const std::u32string &command);
    int getHistoryIndex() const;
    void setHistoryIndex(int index);
    const std::filesystem::path &getCurrentDirectory() const;
    void setCurrentDirectory(const std::filesystem::path &path);

private:
    std::vector<std::u32string> m_commandHistory;
    int m_historyIndex = -1;
    std::filesystem::path m_currentDirectory = std::filesystem::current_path();
};
