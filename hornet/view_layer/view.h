#pragma once

#include <QObject>

struct EditorKeyPressDTO;
struct EditorCursorPosDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
struct WindowDTO;

struct NumberDTO;
class Window;

class View : public QObject {
    Q_OBJECT

public:
    explicit View(const WindowDTO& initialState, QObject* parent = nullptr);
    void show();
    void displayNumber(const NumberDTO& dto);
    void showError(const QString& message);
    void updateEditorState(const EditorViewStateDTO &dto);
    void updateEditorCursorPos(const EditorCursorPosDTO &dto);

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO& dto);
    void debugRequested();
    void closeClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorCursorPosChanged(const EditorCursorPosDTO &dto);
    void editorKeyPressed(const EditorKeyPressDTO &dto);

private:
    Window* m_window;
};
