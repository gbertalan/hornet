#pragma once

#include <QObject>
#include <QDebug>

class IModelAccessRead;
class NumberService;
class WindowService;
class View;
struct NumberDTO;

class Control : public QObject {
    Q_OBJECT

public:
    explicit Control(IModelAccessRead& modelAccess, NumberService& service, WindowService& windowService, View& view);
    void init();

public slots:
    void onButtonClicked();
    void onDebugRequested();
    void onWindowStateChanged(int x, int y, int width, int height, bool isFullscreen);

private:
    IModelAccessRead& m_modelAccess;
    NumberService& m_service;
    WindowService& m_windowService;
    View& m_view;
    void printModel() const;
};
