#include <QApplication>
#include "model_layer/modelaccess.h"
#include "service_layer/numberservice.h"
#include "view_layer/view.h"
#include "control_layer/control.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ModelAccess modelAccess;
    NumberService numberService(modelAccess);
    View view;
    // add services to Control here only what is needed for the main control, as for different parts of the app we can have different Control layer classes.
    Control control(modelAccess, numberService, view);

    QObject::connect(&view, &View::buttonClicked, &control, &Control::onButtonClicked);

    control.init();
    view.show();

    return app.exec();
}
