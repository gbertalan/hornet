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
    void addTerminalLine(const TerminalLine &line);
    const std::vector<TerminalLine> &getTerminalLines() const;
    std::u32string getCurrentPrompt() const;
    const std::filesystem::path &getCurrentDirectory() const;
    void setCurrentDirectory(const std::filesystem::path &path);
    void updateTerminalLineDirectory(int index, const std::filesystem::path &directory);

private:
    EditorService &m_editorService;
    TerminalModel m_terminalModel;
};
