#pragma once
#include <QObject>
class IModelAccessRead;
class EditorService;
class View;
struct EditorVisibleLinesDTO;
struct EditorCursorPosDTO;
struct EditorKeyPressDTO;
class EditorControl : public QObject
{
    Q_OBJECT
public:
    explicit EditorControl(IModelAccessRead &modelAccess, EditorService &editorService, View &view);
    /**
 * @brief sendStateToEditor Retrieves visible lines (and metadata) from Model and sends
 * them to View
 */
    void sendStateToEditor(const QVector<QString> &terminalPrompts = QVector<QString>{});
    void sendCursorPosToEditor();
    void handleEditorKeyPress(const EditorKeyPressDTO &dto);

private:
    IModelAccessRead &m_modelAccess;
    EditorService &m_editorService;
    View &m_view;
};
