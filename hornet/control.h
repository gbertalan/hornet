#pragma once
#include <QObject>
#include "view_layer/view.h"
#include "model_layer/modelaccess.h"
#include "service_layer/projectservice.h"

class Control : public QObject {
    Q_OBJECT
public:
    Control(View &view, ModelAccess &modelAccess, ProjectService &service);
    void init();

public slots:
    void onButtonClicked();

private:
    View &m_view;
    ModelAccess &m_modelAccess;
    ProjectService &m_service;
};

