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
    const std::filesystem::path &retrieveCurrentDirectory() const;
    void storeCurrentDirectory(const std::filesystem::path &path);
    const std::vector<TerminalPromptAndDir> &retrieveTerminalPromptAndDirs() const;
    void addTerminalPromptAndDir(const TerminalPromptAndDir &terminalPromptAndDir);
    void updateLineDirectory(int index,
                             const std::filesystem::path &directory,
                             const std::u32string &prompt);
    void removeTerminalPromptAndDir(int index);

private:
    std::filesystem::path m_currentDirectory = std::filesystem::current_path();
    std::vector<TerminalPromptAndDir> m_terminalPromptAndDirs;
};
