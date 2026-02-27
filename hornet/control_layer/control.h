#pragma once

#include <QObject>

class IModelAccessRead;
class NumberService;
class View;
struct NumberDTO;

class Control : public QObject {
    Q_OBJECT

public:
    explicit Control(IModelAccessRead& modelAccess, NumberService& service, View& view);
    void init();

public slots:
    void onButtonClicked();

private:
    IModelAccessRead& m_modelAccess;
    NumberService& m_service;
    View& m_view;
};
