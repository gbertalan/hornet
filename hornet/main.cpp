#include <QApplication>
#include "control_layer/control.h"
#include "model_layer/modelaccess.h"
#include "service_layer/editorservice.h"
#include "service_layer/gridservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"
#include <service_layer/terminalservice.h>

/*
 * 
 * I looked through the code, and I don't like how it is designed.
 * Particularly how the boxes are sent over.
 * I want to send the box data separately in its DTO, not in gridViewStateDTO.
 * And I want to send one box data at a time, so the DTO would identify which box we are
 * dealing with, wether we delete or create or modify, and wether we create or modify what the parameter values (size, location) are.
 * I guess crud operations.
 * Another issue I see is that BoxModel is used in the DTO and in View as well.
 * I don't like that. We are sending a Model all the way to View. We should just send the box info, 
 * as descibed ealier.
 * Let's design it first, and let's do the work step by step only after I confirm the plan.
 * 
 * I like the BoxModel class.
 * 
 * I think in grid.cpp we don't need FontAtlas and FontRenderer. Currently we pass them to
 * canvasPainter. Instead, we should just instantiate them there, in CanvasPainter.
 * But don't put it into drawBoxes function, because I might have more functions in the CanvasPainter
 * file that will do text rendering.
 * 
 * 
 * /

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

    WindowService windowService(modelAccess);
    EditorService editorService(modelAccess);
    TerminalService terminalService(modelAccess, editorService);
    GridService gridService(modelAccess);

    int x = modelAccess.getWindowModel().getX();
    int y = modelAccess.getWindowModel().getY();
    int width = modelAccess.getWindowModel().getWidth();
    int height = modelAccess.getWindowModel().getHeight();
    bool isFullscreen = modelAccess.getWindowModel().isFullscreen();
    WindowDTO initialState{x, y, width, height, isFullscreen};

    View view(initialState);

    Control control(modelAccess, windowService, editorService, terminalService, gridService, view);

    // connect(sender, signal, receiver, slot)
    QObject::connect(&view, &View::windowStateChanged, &control, &Control::onWindowStateChanged);
    QObject::connect(&view, &View::closeClicked, &app, &QApplication::quit);
    QObject::connect(&view, &View::editorStateChanged, &control, &Control::onEditorStateChanged);
    QObject::connect(&view,
                     &View::editorCursorPosChanged,
                     &control,
                     &Control::onEditorCursorPosChanged);
    QObject::connect(&view, &View::editorKeyPressed, &control, &Control::onEditorKeyPressed);
    QObject::connect(&view, &View::gridZoomChanged, &control, &Control::onGridZoomChanged);
    QObject::connect(&view, &View::gridDragged, &control, &Control::onGridDrag);
    QObject::connect(&view, &View::boxDragged, &control, &Control::onBoxDragged);

    control.init();

    view.show();

    return app.exec();
}
