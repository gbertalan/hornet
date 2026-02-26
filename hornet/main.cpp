#include <QApplication>
#include "view_layer/view.h"
#include "model_layer/numbermodel.h"
#include "service_layer/projectservice.h"
#include "control.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ModelAccess modelAccess;
    ProjectService service;
    View view;
    Control control(&view, &modelAccess, &service);

    control.init();
    view.show();

    return app.exec();
}
