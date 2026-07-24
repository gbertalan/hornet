#include "control.h"
#include "model_layer/editormodel.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/terminalmodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/gridservice.h"
#include "service_layer/terminalservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/boxselecteddto.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"

#include <shared/dto_model_to_view/editorviewstatedto.h>

Control::Control(IModelAccessRead &modelAccess,
                 WindowService &windowService,
                 EditorService &editorService,
                 TerminalService &terminalService,
                 GridService &gridService,
                 View &view)
    : m_modelAccess(modelAccess)
    , m_editorService(editorService)
    , m_terminalService(terminalService)
    , m_gridService(gridService)
    , m_windowControl(modelAccess, windowService, view)
    , m_editorControl(modelAccess, editorService, view)
    , m_terminalControl(modelAccess, editorService, terminalService)
    , m_gridControl(modelAccess, gridService, view)
{}

void Control::init()
{
    m_windowControl.init();
    m_editorControl.init();
    m_terminalControl.init();
    m_gridControl.init();
}

void Control::onWindowStateChanged(const WindowDTO &dto)
{
    m_windowControl.dispatchWindowStateChanged(dto);
}

void Control::onEditorStateChanged(const EditorVisibleLinesDTO &dto)
{
    m_editorService.storeEditorState(dto);
    m_editorControl.sendStateToEditor(
        m_modelAccess.getEditorModel().isTerminal() ? buildTerminalPrompts() : QVector<QString>{});
}

void Control::onEditorCursorPosChanged(const EditorCursorPosDTO &dto)
{
    m_editorService.storeCursorPos(dto);
    if (m_modelAccess.getEditorModel().isTerminal())
        m_terminalControl.dispatchEditorCursorPosChanged(dto);
    m_editorControl.sendCursorPosToEditor();
}

void Control::onEditorKeyPressed(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlD) {
        onDebugRequested();
        return;
    }
    if (m_modelAccess.getEditorModel().isTerminal()
        && m_terminalControl.dispatchTerminalKeyPress(dto)) {
        m_editorControl.sendStateToEditor(buildTerminalPrompts());
        m_editorControl.sendCursorPosToEditor();
        return;
    }
    int lineCountBefore = m_modelAccess.getEditorModel().getNoOfLines();
    int cursorYBefore = m_modelAccess.getEditorModel().getCursorY();
    m_editorControl.dispatchEditorKeyPress(dto);
    if (m_modelAccess.getEditorModel().isTerminal())
        m_terminalControl.removePromptForDeletedLine(lineCountBefore, cursorYBefore);
    m_editorControl.sendStateToEditor(
        m_modelAccess.getEditorModel().isTerminal() ? buildTerminalPrompts() : QVector<QString>{});
    m_editorControl.sendCursorPosToEditor();
}

void Control::onGridZoomChanged(const GridZoomDTO &dto)
{
    m_gridControl.dispatchGridZoomChange(dto);
}

void Control::onGridDrag(const GridDragDTO &dto)
{
    m_gridControl.dispatchGridDrag(dto);
}

void Control::onBoxDragged(const BoxDragDTO &dto)
{
    m_gridControl.dispatchBoxDrag(dto);
}

QVector<QString> Control::buildTerminalPrompts() const
{
    QVector<QString> terminalPrompts;
    const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
        = m_modelAccess.getTerminalModel().getTerminalPromptAndDirs();
    for (const TerminalPromptAndDir &line : terminalPromptAndDirs)
        terminalPrompts.push_back(
            QString::fromUcs4(reinterpret_cast<const char32_t *>(line.prompt.c_str()),
                              static_cast<int>(line.prompt.size())));
    return terminalPrompts;
}

void Control::onBoxSelected(const BoxSelectedDTO &dto)
{
    const BoxContentDTO boxContent = m_gridService.retrieveBoxContent(dto.boxId);

    std::vector<std::u32string> bodyLinesAsU32;
    bodyLinesAsU32.reserve(boxContent.bodyLines.size());
    for (const QString &line : boxContent.bodyLines)
        bodyLinesAsU32.push_back(convertQStringToU32String(line));

    m_editorService.storeTextLines(bodyLinesAsU32, "txt");
    m_editorService.setIsTerminal(false);
    m_editorControl.sendStateToEditor();
    m_editorControl.sendSettingsToEditor();
}

std::u32string Control::convertQStringToU32String(const QString &text) const
{
    const QVector<uint> ucs4 = text.toUcs4();
    return std::u32string(ucs4.begin(), ucs4.end());
}

void Control::onDebugRequested()
{
#ifdef QT_DEBUG
    printModel();
#endif
}

void Control::printModel() const
{
    const WindowModel &windowModel = m_modelAccess.getWindowModel();
    const EditorModel &editorModel = m_modelAccess.getEditorModel();
    const GridModel &gridModel = m_modelAccess.getGridModel();
    qDebug() << "===" << " MODEL STATE" << debugPrintCounter << "===";
    qDebug() << "WindowModel:";
    qDebug() << "    " << "x:" << windowModel.getX() << "y:" << windowModel.getY()
             << "width:" << windowModel.getWidth() << "height:" << windowModel.getHeight()
             << "fullscreen:" << windowModel.isFullscreen();

    qDebug() << "EditorModel:";
    qDebug() << "    " << "noOfVisibleLines:" << editorModel.getNoOfVisibleLines()
             << "topLineIndex:" << editorModel.getTopLineIndex();
    qDebug() << "    " << "noOfLines:" << editorModel.getNoOfLines()
             << "noOfCharsOfLongestLine:" << editorModel.getNoOfCharsOfLongestLine()
             << "fileType:" << editorModel.getFileType();
    qDebug() << "    " << "cursorX:" << editorModel.getCursorX()
             << "cursorY:" << editorModel.getCursorY();

    qDebug() << "GridModel:";
    qDebug() << "    " << "zoomLevel:" << gridModel.getZoomLevel()
             << "gridGap:" << gridModel.getGridGap() << "offset:" << gridModel.getOffset();

    qDebug() << "=== MODEL STATE END ===";
    qDebug() << "";
    debugPrintCounter++;
}
