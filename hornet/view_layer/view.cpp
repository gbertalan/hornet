#include "view_layer/view.h"
#include "view_layer/window.h"

#include <QMessageBox>

View::View(const WindowDTO& initialState, QObject* parent) : QObject(parent) {
    m_window = new Window(initialState);
    connect(m_window, &Window::buttonClicked, this, &View::buttonClicked);
    connect(m_window, &Window::windowStateChanged, this, &View::windowStateChanged);
    connect(m_window, &Window::windowCloseClicked, this, &View::windowCloseClicked);
    connect(m_window, &Window::editorStateChanged, this, &View::editorStateChanged);
    connect(m_window, &Window::editorCursorPosChanged, this, &View::editorCursorPosChanged);
    connect(m_window, &Window::editorKeyPressed, this, &View::editorKeyPressed);
    connect(m_window, &Window::gridZoomChanged, this, &View::gridZoomChanged);
    connect(m_window, &Window::gridDragged, this, &View::gridDragged);
    connect(m_window, &Window::boxDragged, this, &View::boxDragged);
    connect(m_window, &Window::boxSelected, this, &View::boxSelected);
    connect(m_window, &Window::boxResized, this, &View::boxResized);
    connect(m_window, &Window::boxUnloadRequested, this, &View::boxUnloadRequested);
    connect(m_window, &Window::boxListPageRequested, this, &View::boxListPageRequested);
    connect(m_window, &Window::popupBoxListPageRequested, this, &View::popupBoxListPageRequested);
    connect(m_window, &Window::fileLoaderLoadRequested, this, &View::fileLoaderLoadRequested);
    connect(m_window,
            &Window::scriptRunnerBoxRunRequested,
            this,
            &View::scriptRunnerBoxRunRequested);
    connect(m_window, &Window::scriptRunnerRunRequested, this, &View::scriptRunnerRunRequested);
    connect(m_window, &Window::projectSaverSaveRequested, this, &View::projectSaverSaveRequested);
}

void View::show() {
    m_window->show();
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

void View::updateGridViewState(const GridViewStateDTO &dto)
{
    m_window->updateGridViewState(dto);
}

void View::updateFileName(const QString &fileName)
{
    m_window->updateFileName(fileName);
}

void View::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_window->updateBoxListPage(dto);
}

void View::updatePopupBoxListPage(const BoxListPageDTO &dto)
{
    m_window->updatePopupBoxListPage(dto);
}

void View::updateProjectSaverSaveResult(const QString &message)
{
    m_window->updateProjectSaverSaveResult(message);
}

void View::updateCurrentBoxId(int boxId)
{
    m_window->updateCurrentBoxId(boxId);
}
