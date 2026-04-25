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

std::u32string TerminalService::getPrompt() const
{
    std::string segment = m_terminalModel.getCurrentDirectory().filename().string();
    if (segment.empty())
        segment = m_terminalModel.getCurrentDirectory().string();
    std::u32string prompt;
    for (unsigned char c : segment)
        prompt += static_cast<char32_t>(c);
    return prompt;
}

const std::vector<std::u32string> &TerminalService::getLinePrompts() const
{
    return m_terminalModel.getLinePrompts();
}

void TerminalService::addLinePrompt(const std::u32string &prompt)
{
    m_terminalModel.addLinePrompt(prompt);
}
