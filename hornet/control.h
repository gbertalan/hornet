#pragma once
#include <QObject>
#include "view.h"
#include "modelaccess.h"
#include "projectservice.h"

class Control : public QObject {
    Q_OBJECT
public:
    Control(View *view, ModelAccess *modelAccess, ProjectService *service, QObject *parent = nullptr);
    void init();

private slots:
    void onButtonClicked();

private:
    View *m_view;
    ModelAccess *m_modelAccess;
    ProjectService *m_service;
};
