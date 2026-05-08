#include "view_layer/view.h"
#include "view_layer/window.h"

#include <QMessageBox>

View::View(const WindowDTO& initialState, QObject* parent) : QObject(parent) {
    m_window = new Window(initialState);
    connect(m_window, &Window::buttonClicked, this, &View::buttonClicked);
    connect(m_window, &Window::windowStateChanged, this, &View::windowStateChanged);
    connect(m_window, &Window::closeClicked, this, &View::closeClicked);
    connect(m_window, &Window::editorStateChanged, this, &View::editorStateChanged);
    connect(m_window, &Window::editorCursorPosChanged, this, &View::editorCursorPosChanged);
    connect(m_window, &Window::editorKeyPressed, this, &View::editorKeyPressed);
    connect(m_window, &Window::gridZoomChanged, this, &View::gridZoomChanged);
}

void View::show() {
    m_window->show();
}

void View::showError(const QString& message) {
    QMessageBox::warning(m_window, "Error", message, QMessageBox::Ok);
}

void View::updateEditorState(const EditorViewStateDTO &dto)
{
    m_window->updateEditorState(dto);
}

void View::updateEditorCursorPos(const EditorCursorPosDTO &dto)
{
    m_window->updateEditorCursorPos(dto);
}

void View::updateEditorSettings(const EditorSettingsDTO &dto)
{
    m_window->updateEditorSettings(dto);
}
