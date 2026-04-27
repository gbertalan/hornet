#pragma once

#include <filesystem>

#include <model_layer/terminalmodel.h>

class EditorService;
struct EditorKeyPressDTO;

class TerminalService
{
public:
    explicit TerminalService(EditorService &editorService);
    void initialize();
    void addTerminalPromptAndDir(const TerminalPromptAndDir &promptAndDir);
    const std::vector<TerminalPromptAndDir> &getTerminalPromptAndDirs() const;
    std::u32string getCurrentPrompt() const;
    const std::filesystem::path &getCurrentDirectory() const;
    void setCurrentDirectory(const std::filesystem::path &path);
    void updateTerminalLineDirectory(int index, const std::filesystem::path &directory);
    void removeTerminalPromptAndDir(int index);

private:
    EditorService &m_editorService;
    TerminalModel m_terminalModel;
};
