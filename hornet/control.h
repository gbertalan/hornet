#pragma once
#include <QObject>
#include "view_layer/view.h"
#include "model_layer/numbermodel.h"
#include "service_layer/projectservice.h"

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
