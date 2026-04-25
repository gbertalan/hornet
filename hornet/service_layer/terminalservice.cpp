#include "terminalservice.h"
#include <iostream>

TerminalService::TerminalService(EditorService &editorService)
    : m_editorService(editorService)
{}

void TerminalService::navigateHistory(EditorKeyPressDTO::SpecialKey direction) {}

void TerminalService::executeCommand()
{
    std::cout << "executeCommand called" << std::endl;
}
