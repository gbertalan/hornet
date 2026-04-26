#pragma once
#include <filesystem>
#include <string>
#include <vector>

struct TerminalLine
{
    std::u32string prompt;
    std::filesystem::path directory;
};

class TerminalModel
{
public:
    const std::vector<std::u32string> &getCommandHistory() const;
    void addToHistory(const std::u32string &command);
    int getHistoryIndex() const;
    void setHistoryIndex(int index);
    const std::filesystem::path &getCurrentDirectory() const;
    void setCurrentDirectory(const std::filesystem::path &path);
    const std::vector<TerminalLine> &getTerminalLines() const;
    void addTerminalLine(const TerminalLine &line);
    void updateLineDirectory(int index,
                             const std::filesystem::path &directory,
                             const std::u32string &prompt);

private:
    std::vector<std::u32string> m_commandHistory;
    int m_historyIndex = -1;
    std::filesystem::path m_currentDirectory = std::filesystem::current_path();
    std::vector<TerminalLine> m_terminalLines;
};
