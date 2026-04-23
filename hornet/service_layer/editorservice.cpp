#include "editorservice.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/editorvisiblelinesdto.h"

#include <qdebug.h>

EditorService::EditorService(IModelAccessReadWrite &modelAccess)
    : m_modelAccess(modelAccess)
{}

void EditorService::storeEditorState(const EditorVisibleLinesDTO &dto)
{
    m_modelAccess.getEditorModel().setNoOfVisibleLines(dto.noOfVisibleLines);
    m_modelAccess.getEditorModel().setTopLineIndex(dto.topLineIndex);
}

std::vector<std::u32string> EditorService::retrieveActiveLines()
{
    std::vector<std::u32string> visibleLines;
    int topLineIndex = m_modelAccess.getEditorModel().getTopLineIndex();
    int noOfVisLines = m_modelAccess.getEditorModel().getNoOfVisibleLines();
    std::vector<std::u32string> allLines = m_modelAccess.getEditorModel().getTextLines();
    int lastLineIndex = std::min(topLineIndex + noOfVisLines, static_cast<int>(allLines.size()));
    for (int i = topLineIndex; i < lastLineIndex; ++i) {
        visibleLines.push_back(allLines.at(i));
    }
    return visibleLines;
}

void EditorService::setTextLines(std::vector<std::u32string> textLines, std::string fileType)
{
    m_modelAccess.getEditorModel().setTextLines(std::move(textLines));
    m_modelAccess.getEditorModel().setFileType(fileType);
}

void EditorService::storeCursorPos(const EditorCursorPosDTO &dto)
{
    std::vector<std::u32string> allLines = m_modelAccess.getEditorModel().getTextLines();
    int noOfLines = m_modelAccess.getEditorModel().getNoOfLines();
    int cursorY = std::min(dto.cursorY, noOfLines - 1);
    int lineLength = static_cast<int>(allLines.at(cursorY).length());
    int cursorX = std::min(dto.cursorX, lineLength);
    m_modelAccess.getEditorModel().setCursor(cursorX, cursorY);
}

void EditorService::insertCharacter(char32_t character)
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    lines.at(cursorY).insert(cursorX, 1, character);
    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(cursorX + 1, cursorY);
}

void EditorService::moveCursor(const EditorKeyPressDTO &dto)
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    int noOfLines = static_cast<int>(lines.size());

    switch (dto.specialKey) {
    case EditorKeyPressDTO::SpecialKey::Right:
        if (dto.ctrl)
            moveCursorWordRight(lines, cursorX, cursorY);
        else if (cursorX < static_cast<int>(lines.at(cursorY).length()))
            cursorX++;
        else if (cursorY < noOfLines - 1) {
            cursorY++;
            cursorX = 0;
        }
        break;

    case EditorKeyPressDTO::SpecialKey::Left:
        if (dto.ctrl)
            moveCursorWordLeft(lines, cursorX, cursorY);
        else if (cursorX > 0)
            cursorX--;
        else if (cursorY > 0) {
            cursorY--;
            cursorX = static_cast<int>(lines.at(cursorY).length());
        }
        break;
    case EditorKeyPressDTO::SpecialKey::Up:
        if (cursorY > 0) {
            cursorY--;
            cursorX = std::min(cursorX, static_cast<int>(lines.at(cursorY).length()));
        }
        break;
    case EditorKeyPressDTO::SpecialKey::Down:
        if (cursorY < noOfLines - 1) {
            cursorY++;
            cursorX = std::min(cursorX, static_cast<int>(lines.at(cursorY).length()));
        }
        break;
    case EditorKeyPressDTO::SpecialKey::Home:
        cursorX = 0;
        break;
    case EditorKeyPressDTO::SpecialKey::End:
        cursorX = static_cast<int>(lines.at(cursorY).length());
        break;
    case EditorKeyPressDTO::SpecialKey::PageUp:
        cursorY = std::max(0, cursorY - m_modelAccess.getEditorModel().getNoOfVisibleLines());
        cursorX = std::min(cursorX, static_cast<int>(lines.at(cursorY).length()));
        break;
    case EditorKeyPressDTO::SpecialKey::PageDown:
        cursorY = std::min(noOfLines - 1,
                           cursorY + m_modelAccess.getEditorModel().getNoOfVisibleLines());
        cursorX = std::min(cursorX, static_cast<int>(lines.at(cursorY).length()));
        break;
    default:
        break;
    }

    m_modelAccess.getEditorModel().setCursor(cursorX, cursorY);
}

void EditorService::deleteCharacter(const EditorKeyPressDTO &dto)
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    int noOfLines = static_cast<int>(lines.size());

    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace) {
        if (cursorX > 0) {
            lines.at(cursorY).erase(cursorX - 1, 1);
            cursorX--;
        } else if (cursorY > 0) {
            int prevLineLength = static_cast<int>(lines.at(cursorY - 1).length());
            lines.at(cursorY - 1) += lines.at(cursorY);
            lines.erase(lines.begin() + cursorY);
            cursorY--;
            cursorX = prevLineLength;
        }
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Delete) {
        if (cursorX < static_cast<int>(lines.at(cursorY).length())) {
            lines.at(cursorY).erase(cursorX, 1);
        } else if (cursorY < noOfLines - 1) {
            lines.at(cursorY) += lines.at(cursorY + 1);
            lines.erase(lines.begin() + cursorY + 1);
        }
    }

    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(cursorX, cursorY);
}

void EditorService::insertNewLine()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();

    std::u32string &currentLine = lines.at(cursorY);
    std::u32string newLine = currentLine.substr(cursorX);
    currentLine = currentLine.substr(0, cursorX);
    lines.insert(lines.begin() + cursorY + 1, newLine);

    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(0, cursorY + 1);
}

void EditorService::insertTab()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    lines.at(cursorY).insert(cursorX, 4, U' ');
    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(cursorX + 4, cursorY);
}

enum class CharCategory {
    Whitespace,
    UppercaseLetter,
    LowercaseLetter,
    Digit,
    Underscore,
    Punctuation
};

static CharCategory categorize(char32_t ch)
{
    if (ch == U' ' || ch == U'\t')
        return CharCategory::Whitespace;
    if (ch == U'_')
        return CharCategory::Underscore;
    if (ch >= U'A' && ch <= U'Z')
        return CharCategory::UppercaseLetter;
    if (ch >= U'a' && ch <= U'z')
        return CharCategory::LowercaseLetter;
    if (ch >= U'0' && ch <= U'9')
        return CharCategory::Digit;
    return CharCategory::Punctuation;
}

void EditorService::moveCursorWordRight(std::vector<std::u32string> &lines,
                                        int &cursorX,
                                        int &cursorY)
{
    int noOfLines = static_cast<int>(lines.size());
    if (cursorX == static_cast<int>(lines.at(cursorY).length())) {
        if (cursorY < noOfLines - 1) {
            cursorY++;
            cursorX = 0;
        }
        return;
    }
    CharCategory startCategory = categorize(lines.at(cursorY).at(cursorX));
    if (startCategory == CharCategory::Whitespace) {
        while (cursorX < static_cast<int>(lines.at(cursorY).length())
               && categorize(lines.at(cursorY).at(cursorX)) == CharCategory::Whitespace)
            cursorX++;
    } else if (startCategory == CharCategory::LowercaseLetter
               || startCategory == CharCategory::Digit
               || startCategory == CharCategory::Underscore) {
        while (cursorX < static_cast<int>(lines.at(cursorY).length())) {
            CharCategory cat = categorize(lines.at(cursorY).at(cursorX));
            if (cat == CharCategory::UppercaseLetter || cat == CharCategory::Whitespace
                || cat == CharCategory::Punctuation)
                break;
            cursorX++;
        }
    } else if (startCategory == CharCategory::UppercaseLetter) {
        cursorX++;
        while (cursorX < static_cast<int>(lines.at(cursorY).length())) {
            CharCategory cat = categorize(lines.at(cursorY).at(cursorX));
            if (cat != CharCategory::UppercaseLetter) {
                if (cat == CharCategory::LowercaseLetter) {
                    while (cursorX < static_cast<int>(lines.at(cursorY).length())
                           && categorize(lines.at(cursorY).at(cursorX))
                                  == CharCategory::LowercaseLetter)
                        cursorX++;
                }
                break;
            }
            cursorX++;
        }
    } else {
        while (cursorX < static_cast<int>(lines.at(cursorY).length())
               && categorize(lines.at(cursorY).at(cursorX)) == CharCategory::Punctuation)
            cursorX++;
    }
}

void EditorService::moveCursorWordLeft(std::vector<std::u32string> &lines,
                                       int &cursorX,
                                       int &cursorY)
{
    if (cursorX == 0) {
        if (cursorY > 0) {
            cursorY--;
            cursorX = static_cast<int>(lines.at(cursorY).length());
        }
        return;
    }
    cursorX--;
    CharCategory startCategory = categorize(lines.at(cursorY).at(cursorX));
    if (startCategory == CharCategory::Whitespace) {
        while (cursorX > 0
               && categorize(lines.at(cursorY).at(cursorX - 1)) == CharCategory::Whitespace)
            cursorX--;
    } else if (startCategory == CharCategory::LowercaseLetter) {
        while (cursorX > 0) {
            CharCategory cat = categorize(lines.at(cursorY).at(cursorX - 1));
            if (cat == CharCategory::Whitespace || cat == CharCategory::Punctuation
                || cat == CharCategory::UppercaseLetter)
                break;
            cursorX--;
        }
    } else if (startCategory == CharCategory::UppercaseLetter) {
        while (cursorX > 0) {
            CharCategory cat = categorize(lines.at(cursorY).at(cursorX - 1));
            if (cat != CharCategory::UppercaseLetter)
                break;
            cursorX--;
        }
    } else if (startCategory == CharCategory::Digit || startCategory == CharCategory::Underscore) {
        while (cursorX > 0) {
            CharCategory cat = categorize(lines.at(cursorY).at(cursorX - 1));
            if (cat == CharCategory::Whitespace || cat == CharCategory::Punctuation
                || cat == CharCategory::UppercaseLetter)
                break;
            cursorX--;
        }
    } else {
        while (cursorX > 0
               && categorize(lines.at(cursorY).at(cursorX - 1)) == CharCategory::Punctuation)
            cursorX--;
    }
}
