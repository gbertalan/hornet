#pragma once

#include <QDebug>
#include <QObject>

class IModelAccessRead;
class NumberService;
class WindowService;
class EditorService;
class View;
struct NumberDTO;
struct WindowDTO;
struct EditorVisibleLinesDTO;
struct EditorUserInputDTO;

class Control : public QObject
{
    Q_OBJECT

public:
    explicit Control(IModelAccessRead &modelAccess,
                     NumberService &service,
                     WindowService &windowService,
                     EditorService &editorService,
                     View &view);
    void init();

public slots:
    void onButtonClicked();
    void onDebugRequested();
    void onWindowStateChanged(const WindowDTO &dto);
    void onEditorStateChanged(const EditorVisibleLinesDTO &dto);
    void onEditorUserInputOccured(const EditorUserInputDTO &dto);

private:
    void sendTextToEditor();

    IModelAccessRead &m_modelAccess;
    NumberService &m_service;
    WindowService &m_windowService;
    EditorService &m_editorService;
    View &m_view;
    void printModel() const;
    mutable int debugPrintCounter = 0; // mutable, to use in const function
};
