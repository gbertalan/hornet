#ifndef TERMINALMODEL_H
#define TERMINALMODEL_H

#include <string>
#include <vector>

class TerminalModel
{
public:
    TerminalModel();
    std::vector<std::u32string> commandHistory;
    int historyIndex = -1;
};

#endif // TERMINALMODEL_H
