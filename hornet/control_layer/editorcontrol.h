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
    void init();

    // -- Sending data to the editor ----------------------
    /**
    * @brief sendStateToEditor Retrieves visible lines (and metadata) from Model,
    * packages them in a DTO and calls View's API with this DTO.
    */
    void sendStateToEditor(const QVector<QString> &terminalPrompts = QVector<QString>{});
    /**
     * @brief sendCursorPosToEditor Retrieves cursorX and cursorY from Model, 
     * packages them in a DTO and calls View's API with this DTO.
     */
    void sendCursorPosToEditor();
    void sendSelectionToEditor();
    /**
     * @brief sendSettingsToEditor Retrieves editor setting (line height, font scale, isTerminal) from Model,
     * packages them in a DTO and calls View's API with this DTO.
     */
    void sendSettingsToEditor();
    // -- Sending data to the editor - end ---------------

    void dispatchEditorKeyPress(const EditorKeyPressDTO &dto);
    void selectAll();

private:
    IModelAccessRead &m_modelAccess;
    EditorService &m_editorService;
    View &m_view;
};
