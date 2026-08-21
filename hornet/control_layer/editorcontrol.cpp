#include "editorcontrol.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_read.h"
#include "service_layer/editorservice.h"
#include "shared/dto_model_to_view/editorviewstatedto.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "view_layer/view.h"

#include "shared/dto_view_to_model/editorselectiondto.h"
#include <qdebug.h>
#include <shared/dto_bidirectional/editorsettingsdto.h>

EditorControl::EditorControl(IModelAccessRead &modelAccess, EditorService &editorService, View &view)
    : m_modelAccess(modelAccess)
    , m_editorService(editorService)
    , m_view(view)
{}

void EditorControl::init()
{
    m_editorService.init();
    sendSettingsToEditor();
    sendCursorPosToEditor();
}

// -- Sending data to the editor ----------------------

void EditorControl::sendStateToEditor(const QVector<QString> &terminalPrompts)
{
    std::vector<std::u32string> lines = m_editorService.retrieveActiveLines();
    if (lines.empty())
        return;
    int noOfAllLines = m_modelAccess.getEditorModel().getNoOfLines();
    int noOfCharsOfLongestLine = m_modelAccess.getEditorModel().getNoOfCharsOfLongestLine();
    std::string fileType = m_modelAccess.getEditorModel().getFileType();
    QVector<QString> qLines;
    qLines.reserve(static_cast<int>(lines.size()));
    for (const std::u32string &line : lines)
        qLines.push_back(QString::fromUcs4(reinterpret_cast<const char32_t *>(line.c_str()),
                                           static_cast<int>(line.size())));
    EditorViewStateDTO dto{qLines,
                           noOfAllLines,
                           noOfCharsOfLongestLine,
                           QString::fromStdString(fileType),
                           terminalPrompts};
    m_view.updateEditorState(dto);
}

void EditorControl::sendCursorPosToEditor()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    EditorCursorPosDTO dto{cursorX, cursorY};
    m_view.updateEditorCursorPos(dto);
}

void EditorControl::sendSelectionToEditor()
{
    EditorSelectionDTO dto{m_modelAccess.getEditorModel().getSelectionAnchorX(),
                           m_modelAccess.getEditorModel().getSelectionAnchorY(),
                           m_modelAccess.getEditorModel().getSelectionExtentX(),
                           m_modelAccess.getEditorModel().getSelectionExtentY(),
                           m_modelAccess.getEditorModel().hasSelection()};
    m_view.updateEditorSelection(dto);
}

void EditorControl::sendSettingsToEditor()
{
    EditorSettingsDTO dto{m_modelAccess.getEditorModel().getLineHeight(),
                          m_modelAccess.getEditorModel().getFontScale(),
                          m_modelAccess.getEditorModel().isTerminal()};
    m_view.updateEditorSettings(dto);
}

// -- Sending data to the editor - end ---------------

void EditorControl::dispatchEditorKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.alt)
        return;
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::None) {
        if (dto.ctrl) {
            qDebug() << "EditorControl says CONTROL PRESSED";
            return;
        }
        m_editorService.insertCharacter(dto.key);
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace
               || dto.specialKey == EditorKeyPressDTO::SpecialKey::Delete) {
        if (dto.ctrl) {
            if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace)
                m_editorService.deleteWordLeft();
            else
                m_editorService.deleteWordRight();
        } else {
            m_editorService.deleteCharacter(dto);
        }
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter)
        m_editorService.insertNewLine();
    else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Tab)
        m_editorService.insertTab();
    else
        m_editorService.moveCursor(dto);
}

void EditorControl::selectAll()
{
    m_editorService.selectAll();
}

void EditorControl::copySelection()
{
    m_editorService.copySelection();
}

void EditorControl::pasteFromClipboard()
{
    m_editorService.pasteFromClipboard();
}