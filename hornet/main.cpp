#include <QApplication>
#include "model.h"
#include "view.h"
#include "modelaccess.h"
#include "projectservice.h"
#include "control.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Model model;
    ModelAccess modelAccess(&model);
    ProjectService service;
    View view;
    Control control(&view, &modelAccess, &service);

    control.init();
    view.show();

    return app.exec();
}
