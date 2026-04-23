#pragma once

#include <QObject>

struct EditorUserInputDTO;
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
    void updateEditorLines(const EditorViewStateDTO &dto);

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO& dto);
    void debugRequested();
    void closeClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorUserInputOccured(const EditorUserInputDTO &dto);

private:
    Window* m_window;
};
