#include <QApplication>
#include "view_layer/view.h"
#include "model_layer/modelaccess.h"
#include "service_layer/projectservice.h"
#include "control.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ModelAccess modelAccess;
    ProjectService projectService(modelAccess);
    View view;
    Control control(view, modelAccess, projectService);

    QObject::connect(&view, &View::buttonClicked, &control, &Control::onButtonClicked);

    control.init();
    view.show();

    return app.exec();
}
