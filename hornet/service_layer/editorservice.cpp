#include "editorservice.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
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
