#include <QApplication>
#include "control_layer/control.h"
#include "model_layer/modelaccess.h"
#include "service_layer/editorservice.h"
#include "service_layer/numberservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"

/**
 * @brief main The main function
 *
 * Creates:
 * - QApplication
 * - ModelAccess
 * - Services
 * - View
 * - Control
 * Connects:
 * - View -> Control
 *
 * @param argc No arguments expected
 * @param argv No arguments expected
 * @return app.exec(); This is the last line on purpose. Don't do cleanup after exec() is called.
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ModelAccess modelAccess;

    NumberService numberService(modelAccess);
    WindowService windowService(modelAccess);
    EditorService editorService(modelAccess);

    int x = modelAccess.getWindowModel().getX();
    int y = modelAccess.getWindowModel().getY();
    int width = modelAccess.getWindowModel().getWidth();
    int height = modelAccess.getWindowModel().getHeight();
    bool isFullscreen = modelAccess.getWindowModel().isFullscreen();
    WindowDTO initialState{x, y, width, height, isFullscreen};

    View view(initialState);

    Control control(modelAccess, numberService, windowService, editorService, view);

    // connect(sender, signal, receiver, slot)
    QObject::connect(&view, &View::debugRequested, &control, &Control::onDebugRequested);
    QObject::connect(&view, &View::buttonClicked, &control, &Control::onButtonClicked);
    QObject::connect(&view, &View::windowStateChanged, &control, &Control::onWindowStateChanged);
    QObject::connect(&view, &View::closeClicked, &app, &QApplication::quit);
    QObject::connect(&view, &View::editorStateChanged, &control, &Control::onEditorStateChanged);
    QObject::connect(&view,
                     &View::editorCursorPosChanged,
                     &control,
                     &Control::onEditorCursorPosChanged);

    control.init();

    view.show();

    return app.exec();
}
