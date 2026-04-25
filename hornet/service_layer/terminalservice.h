#pragma once

#include <model_layer/terminalmodel.h>

#include <shared/dto_view_to_model/editorkeypressdto.h>

class EditorService;

class TerminalService
{
public:
    explicit TerminalService(EditorService &editorService);
    void navigateHistory(EditorKeyPressDTO::SpecialKey direction);
    void executeCommand();

private:
    EditorService &m_editorService;
    TerminalModel m_terminalModel;
};
