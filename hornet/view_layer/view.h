#pragma once

#include <QObject>

struct EditorVisibleLinesDTO;
struct EditorTextContentsDTO;
struct WindowDTO;
// #include "shared/dto_view_to_model/windowdto.h"
// #include <shared/dto_model_to_view/editortextcontentsdto.h>
// #include <shared/dto_view_to_model/editorvisiblelinesdto.h>

struct NumberDTO;
class Window;

class View : public QObject {
    Q_OBJECT

public:
    explicit View(const WindowDTO& initialState, QObject* parent = nullptr);
    void show();
    void displayNumber(const NumberDTO& dto);
    void showError(const QString& message);
    void updateEditorLines(const EditorTextContentsDTO &dto);

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO& dto);
    void debugRequested();
    void closeClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);

private:
    Window* m_window;
};
