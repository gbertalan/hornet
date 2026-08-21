#include "editorservice.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QString>
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/editorselectiondto.h"
#include "shared/dto_view_to_model/editorvisiblelinesdto.h"

#include <qdebug.h>

EditorService::EditorService(IModelAccessReadWrite &modelAccess)
    : m_modelAccess(modelAccess)
{}

void EditorService::init()
{
    storeTextLines({U""}, "");
}

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

void EditorService::storeTextLines(std::vector<std::u32string> textLines, std::string fileType)
{
    if (textLines.empty())
        textLines.push_back(U"");
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

void EditorService::storeSelection(const EditorSelectionDTO &dto)
{
    m_modelAccess.getEditorModel().setSelection(dto.anchorX,
                                                dto.anchorY,
                                                dto.extentX,
                                                dto.extentY,
                                                dto.hasSelection);
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
    int oldCursorX = cursorX;
    int oldCursorY = cursorY;
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

    if (dto.shift) {
        int anchorX = m_modelAccess.getEditorModel().hasSelection()
                          ? m_modelAccess.getEditorModel().getSelectionAnchorX()
                          : oldCursorX;
        int anchorY = m_modelAccess.getEditorModel().hasSelection()
                          ? m_modelAccess.getEditorModel().getSelectionAnchorY()
                          : oldCursorY;
        bool hasSel = (anchorX != cursorX || anchorY != cursorY);
        m_modelAccess.getEditorModel().setSelection(anchorX, anchorY, cursorX, cursorY, hasSel);
    } else {
        m_modelAccess.getEditorModel().setSelection(0, 0, 0, 0, false);
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
            shiftMarksForLineDeleted(cursorY);
            cursorY--;
            cursorX = prevLineLength;
        }
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Delete) {
        if (cursorX < static_cast<int>(lines.at(cursorY).length())) {
            lines.at(cursorY).erase(cursorX, 1);
        } else if (cursorY < noOfLines - 1) {
            lines.at(cursorY) += lines.at(cursorY + 1);
            lines.erase(lines.begin() + cursorY + 1);
            shiftMarksForLineDeleted(cursorY + 1);
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
    shiftMarksForLinesInserted(cursorY + 1, 1);
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

static bool isWordChar(char32_t ch)
{
    return (ch >= U'a' && ch <= U'z') || (ch >= U'A' && ch <= U'Z') || (ch >= U'0' && ch <= U'9')
           || (ch == U';') || (ch == U'>') || (ch == U'<');
}

void EditorService::moveCursorWordRight(std::vector<std::u32string> &lines,
                                        int &cursorX,
                                        int &cursorY)
{
    int noOfLines = static_cast<int>(lines.size());
    const std::u32string &line = lines.at(cursorY);
    int lineLen = static_cast<int>(line.length());

    if (cursorX == lineLen) {
        if (cursorY < noOfLines - 1) {
            cursorY++;
            cursorX = 0;
        }
        return;
    }

    if (isWordChar(line.at(cursorX))) {
        if (isupper(line.at(cursorX))) {
            cursorX++;
            while (cursorX < lineLen && islower(line.at(cursorX)))
                cursorX++;
        } else {
            while (cursorX < lineLen && isWordChar(line.at(cursorX)) && !isupper(line.at(cursorX)))
                cursorX++;
        }
    }

    while (cursorX < lineLen && !isWordChar(line.at(cursorX)))
        cursorX++;
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

    const std::u32string &line = lines.at(cursorY);

    while (cursorX > 0 && !isWordChar(line.at(cursorX - 1)))
        cursorX--;

    if (cursorX > 0 && isupper(line.at(cursorX - 1))) {
        while (cursorX > 0 && isupper(line.at(cursorX - 1)))
            cursorX--;
    } else {
        while (cursorX > 0 && isWordChar(line.at(cursorX - 1)) && !isupper(line.at(cursorX - 1)))
            cursorX--;
    }
}

void EditorService::deleteWordLeft()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    int oldCursorX = cursorX;
    int oldCursorY = cursorY;
    moveCursorWordLeft(lines, cursorX, cursorY);

    // If word jump crossed a line boundary, merge the current line into the previous one
    if (cursorY != oldCursorY) {
        lines.at(cursorY) += lines.at(oldCursorY);
        lines.erase(lines.begin() + oldCursorY);
    } else {
        lines.at(cursorY).erase(cursorX, oldCursorX - cursorX);
    }
    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(cursorX, cursorY);
}

void EditorService::deleteWordRight()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    int oldCursorX = cursorX;
    int oldCursorY = cursorY;
    moveCursorWordRight(lines, cursorX, cursorY);

    // If word jump crossed a line boundary, merge the next line in rather than erasing
    if (cursorY != oldCursorY) {
        lines.at(oldCursorY) += lines.at(cursorY);
        lines.erase(lines.begin() + cursorY);
    } else {
        lines.at(cursorY).erase(oldCursorX, cursorX - oldCursorX);
    }

    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(oldCursorX, oldCursorY);
}

bool EditorService::isTerminal() const
{
    return m_modelAccess.getEditorModel().isTerminal();
}

void EditorService::setIsTerminal(bool isTerminal)
{
    m_modelAccess.getEditorModel().setIsTerminal(isTerminal);
}

void EditorService::selectAll()
{
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    int noOfLines = static_cast<int>(lines.size());
    int lastLine = std::max(0, noOfLines - 1);
    int lastCol = static_cast<int>(lines.at(lastLine).length());

    m_modelAccess.getEditorModel().setSelection(0, 0, lastCol, lastLine, true);
    m_modelAccess.getEditorModel().setCursor(lastCol, lastLine);
}

// EditorService — selected text as QString
QString EditorService::getSelectedText() const
{
    if (!m_modelAccess.getEditorModel().hasSelection())
        return QString();

    int startX = m_modelAccess.getEditorModel().getSelectionAnchorX();
    int startY = m_modelAccess.getEditorModel().getSelectionAnchorY();
    int endX = m_modelAccess.getEditorModel().getSelectionExtentX();
    int endY = m_modelAccess.getEditorModel().getSelectionExtentY();
    if (startY > endY || (startY == endY && startX > endX)) {
        std::swap(startX, endX);
        std::swap(startY, endY);
    }

    const std::vector<std::u32string> &lines = m_modelAccess.getEditorModel().getTextLines();
    std::u32string result;
    if (startY == endY) {
        result = lines.at(startY).substr(startX, endX - startX);
    } else {
        result += lines.at(startY).substr(startX);
        result += U'\n';
        for (int row = startY + 1; row < endY; ++row) {
            result += lines.at(row);
            result += U'\n';
        }
        result += lines.at(endY).substr(0, endX);
    }
    return QString::fromUcs4(reinterpret_cast<const char32_t *>(result.data()),
                             static_cast<int>(result.size()));
}

void EditorService::copySelection()
{
    QString text = getSelectedText();
    if (!text.isEmpty())
        QGuiApplication::clipboard()->setText(text);
}

void EditorService::deleteSelectionInternal()
{
    int startX = m_modelAccess.getEditorModel().getSelectionAnchorX();
    int startY = m_modelAccess.getEditorModel().getSelectionAnchorY();
    int endX = m_modelAccess.getEditorModel().getSelectionExtentX();
    int endY = m_modelAccess.getEditorModel().getSelectionExtentY();
    if (startY > endY || (startY == endY && startX > endX)) {
        std::swap(startX, endX);
        std::swap(startY, endY);
    }

    for (int i = 0; i < endY - startY; ++i)
        shiftMarksForLineDeleted(startY + 1);

    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();
    std::u32string merged = lines.at(startY).substr(0, startX) + lines.at(endY).substr(endX);
    lines.erase(lines.begin() + startY, lines.begin() + endY + 1);
    lines.insert(lines.begin() + startY, merged);
    m_modelAccess.getEditorModel().setTextLines(std::move(lines));

    m_modelAccess.getEditorModel().setCursor(startX, startY);
    m_modelAccess.getEditorModel().setSelection(0, 0, 0, 0, false);
}

void EditorService::pasteFromClipboard()
{
    QString clipboardText = QGuiApplication::clipboard()->text();
    if (clipboardText.isEmpty())
        return;

    if (m_modelAccess.getEditorModel().hasSelection())
        deleteSelectionInternal();

    QVector<uint> ucs4 = clipboardText.toUcs4();
    std::vector<std::u32string> pastedLines;
    std::u32string current;
    for (uint ch : ucs4) {
        if (ch == U'\n') {
            pastedLines.push_back(current);
            current.clear();
        } else if (ch != U'\r') {
            current += static_cast<char32_t>(ch);
        }
    }
    pastedLines.push_back(current);

    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    std::vector<std::u32string> lines = m_modelAccess.getEditorModel().getTextLines();

    std::u32string tail = lines.at(cursorY).substr(cursorX);
    std::u32string head = lines.at(cursorY).substr(0, cursorX);

    if (pastedLines.size() == 1) {
        lines.at(cursorY) = head + pastedLines[0] + tail;
        cursorX = static_cast<int>((head + pastedLines[0]).size());
    } else {
        std::vector<std::u32string> newLines;
        newLines.push_back(head + pastedLines.front());
        for (size_t i = 1; i < pastedLines.size() - 1; ++i)
            newLines.push_back(pastedLines[i]);
        newLines.push_back(pastedLines.back() + tail);

        lines.erase(lines.begin() + cursorY);
        lines.insert(lines.begin() + cursorY, newLines.begin(), newLines.end());

        shiftMarksForLinesInserted(cursorY + 1, static_cast<int>(newLines.size()) - 1);

        cursorY += static_cast<int>(pastedLines.size()) - 1;
        cursorX = static_cast<int>(pastedLines.back().size());
    }

    m_modelAccess.getEditorModel().setTextLines(std::move(lines));
    m_modelAccess.getEditorModel().setCursor(cursorX, cursorY);
}

void EditorService::cutSelection()
{
    if (!m_modelAccess.getEditorModel().hasSelection())
        return;
    copySelection();
    deleteSelectionInternal();
}

void EditorService::storeMarks(const std::vector<MarkRange> &marks)
{
    m_modelAccess.getEditorModel().setMarks(marks);
}

void EditorService::shiftMarksForLinesInserted(int atLine, int count)
{
    if (count <= 0)
        return;
    std::vector<MarkRange> marks = m_modelAccess.getEditorModel().getMarks();
    for (MarkRange &mark : marks) {
        if (atLine <= mark.startLine) {
            mark.startLine += count;
            mark.endLine += count;
        } else if (atLine <= mark.endLine) {
            mark.endLine += count;
        }
    }
    m_modelAccess.getEditorModel().setMarks(marks);
}

void EditorService::shiftMarksForLineDeleted(int deletedLine)
{
    std::vector<MarkRange> marks = m_modelAccess.getEditorModel().getMarks();
    std::vector<MarkRange> result;
    for (const MarkRange &mark : marks) {
        if (deletedLine < mark.startLine) {
            result.push_back(MarkRange(mark.startLine - 1, mark.endLine - 1));
        } else if (deletedLine > mark.endLine) {
            result.push_back(mark);
        } else if (mark.startLine == mark.endLine) {
            continue; // the mark's only line was deleted
        } else if (deletedLine > mark.startLine && deletedLine < mark.endLine) {
            result.push_back(MarkRange(mark.startLine, deletedLine - 1));
            result.push_back(MarkRange(deletedLine, mark.endLine - 1));
        } else {
            // deletedLine == mark.startLine or mark.endLine
            result.push_back(MarkRange(mark.startLine, mark.endLine - 1));
        }
    }
    m_modelAccess.getEditorModel().setMarks(result);
}
