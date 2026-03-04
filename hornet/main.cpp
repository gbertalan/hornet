#include <QApplication>
#include "model_layer/modelaccess.h"
#include "service_layer/numberservice.h"
#include "service_layer/windowservice.h"
#include "view_layer/view.h"
#include "control_layer/control.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ModelAccess modelAccess;
    NumberService numberService(modelAccess);
    WindowService windowService(modelAccess);
    View view;
    Control control(modelAccess, numberService, windowService, view);

    QObject::connect(&view, &View::debugRequested, &control, &Control::onDebugRequested);
    QObject::connect(&view, &View::buttonClicked, &control, &Control::onButtonClicked);
    QObject::connect(&view, &View::windowStateChanged, &control, &Control::onWindowStateChanged);

    control.init();
    view.show();

    return app.exec();
}
