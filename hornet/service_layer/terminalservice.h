#ifndef TERMINALSERVICE_H
#define TERMINALSERVICE_H
#include "model_layer/terminalmodel.h"
#include <filesystem>
#include <string>
#include <vector>

class EditorService;
class IModelAccessReadWrite;

class TerminalService
{
public:
    explicit TerminalService(IModelAccessReadWrite &modelAccess, EditorService &editorService);
    void init();
    void addTerminalPromptAndDir(const TerminalPromptAndDir &promptAndDir);
    const std::vector<TerminalPromptAndDir> &getTerminalPromptAndDirs() const;
    std::u32string getCurrentPrompt() const;
    const std::filesystem::path &getCurrentDirectory() const;
    void setCurrentDirectory(const std::filesystem::path &path);
    void updateTerminalLineDirectory(int index, const std::filesystem::path &directory);
    void removeTerminalPromptAndDir(int index);

private:
    IModelAccessReadWrite &m_modelAccess;
    EditorService &m_editorService;
};
#endif // TERMINALSERVICE_H
