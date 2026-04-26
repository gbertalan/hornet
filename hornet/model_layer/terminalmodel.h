#pragma once
#include <filesystem>
#include <string>
#include <vector>

struct TerminalPromptAndDir
{
    std::u32string prompt;
    std::filesystem::path directory;
};

class TerminalModel
{
public:
    const std::filesystem::path &getCurrentDirectory() const;
    void setCurrentDirectory(const std::filesystem::path &path);
    const std::vector<TerminalPromptAndDir> &getTerminalTerminalPromptAndDirs() const;
    void addTerminalPromptAndDir(const TerminalPromptAndDir &terminalPromptAndDir);
    void updateLineDirectory(int index,
                             const std::filesystem::path &directory,
                             const std::u32string &prompt);

private:
    std::filesystem::path m_currentDirectory = std::filesystem::current_path();
    std::vector<TerminalPromptAndDir> m_terminalPromptAndDirs;
};
