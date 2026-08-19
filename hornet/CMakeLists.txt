cmake_minimum_required(VERSION 3.16)
project(hornet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- FreeType (auto-downloaded at configure time) -------------------------
include(FetchContent)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG        VER-2-13-2
    GIT_SHALLOW    TRUE
)
set(FT_DISABLE_ZLIB     TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   TRUE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)
# --------------------------------------------------------------------------

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    model_layer/modelaccess.cpp
    model_layer/windowmodel.cpp
    view_layer/window.cpp
    view_layer/theme.cpp
    view_layer/titlebar.cpp
    view_layer/titlebarbutton.cpp
    view_layer/splitpane.cpp
    view_layer/overlaywidget.cpp
    view_layer/resizehandle.cpp
    view_layer/view.cpp
    view_layer/customscrollbar.cpp

    view_layer/font_renderer/FontAtlas.cpp
    view_layer/font_renderer/FontRenderer.cpp

    control_layer/control.cpp


    service_layer/windowservice.cpp


    resources/resources.qrc
)
set(PROJECT_HEADERS
    model_layer/modelaccess.h
    model_layer/windowmodel.h
    model_layer/imodelaccess_read.h
    model_layer/imodelaccess_readwrite.h
    view_layer/window.h
    view_layer/theme.h
    view_layer/titlebar.h
    view_layer/titlebarbutton.h
    view_layer/splitpane.h
    view_layer/overlaywidget.h
    view_layer/resizehandle.h
    view_layer/view.h
    view_layer/customscrollbar.h

    view_layer/font_renderer/FontAtlas.h
    view_layer/font_renderer/FontRenderer.h

    control_layer/control.h


    service_layer/windowservice.h


    shared/dto_view_to_model/windowdto.h
)
qt_add_executable(hornet
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    view_layer/editor/editor.h view_layer/editor/editor.cpp
    shared/dto_bidirectional/editorsettingsdto.h
    shared/dto_model_to_view/editorviewstatedto.h
    shared/dto_view_to_model/editorvisiblelinesdto.h
    service_layer/editorservice.h service_layer/editorservice.cpp
    model_layer/editormodel.h model_layer/editormodel.cpp
    shared/dto_view_to_model/editorcursorposdto.h
    shared/dto_view_to_model/editorkeypressdto.h
    model_layer/terminalmodel.h model_layer/terminalmodel.cpp
    service_layer/terminalservice.h service_layer/terminalservice.cpp
    control_layer/terminalcontrol.h control_layer/terminalcontrol.cpp
    control_layer/editorcontrol.h control_layer/editorcontrol.cpp
    control_layer/windowcontrol.h control_layer/windowcontrol.cpp
    model_layer/boxmodel.h model_layer/boxmodel.cpp

    control_layer/gridcontrol.h control_layer/gridcontrol.cpp
    model_layer/gridmodel.h model_layer/gridmodel.cpp
    service_layer/gridservice.h service_layer/gridservice.cpp
    view_layer/grid/grid.h view_layer/grid/grid.cpp
    shared/dto_view_to_model/gridzoomdto.h
    shared/dto_model_to_view/gridviewstatedto.h
    shared/dto_view_to_model/griddragdto.h
    view_layer/grid/canvaspainter.h view_layer/grid/canvaspainter.cpp
    shared/dto_model_to_view/boxviewdto.h
    shared/dto_view_to_model/boxdragdto.h
    shared/dto_view_to_model/boxselecteddto.h
    shared/dto_model_to_view/boxcontentdto.h
    model_layer/boxcontenttype.h
    shared/dto_view_to_model/boxresizeedge.h
    shared/dto_view_to_model/boxresizedto.h
    shared/dto_model_to_view/toollinedto.h
    service_layer/toolscriptparser.h service_layer/toolscriptparser.cpp
    shared/dto_model_to_view/toolrectdto.h
    shared/dto_model_to_view/toolcircledto.h
    shared/dto_model_to_view/tooltextdto.h
    shared/dto_model_to_view/toolscriptdto.h
    shared/dto_model_to_view/toolsourcedto.h
    control_layer/toolcontrol.h control_layer/toolcontrol.cpp
    view_layer/titlebarfilenamebutton.h view_layer/titlebarfilenamebutton.cpp
    shared/dto_model_to_view/boxlistentrydto.h
    shared/dto_model_to_view/boxlistpagedto.h
    shared/dto_model_to_view/boxlistpagerequestdto.h
    view_layer/titlebarfiledropdown.h view_layer/titlebarfiledropdown.cpp
    view_layer/mainpopup.h view_layer/mainpopup.cpp
    view_layer/boxlistpanel.h view_layer/boxlistpanel.cpp
    view_layer/fileloaderpanel.h view_layer/fileloaderpanel.cpp
    shared/dto_view_to_model/filepathlistdto.h
    view_layer/scriptrunnerpanel.h view_layer/scriptrunnerpanel.cpp
    view_layer/projectsaverpanel.h view_layer/projectsaverpanel.cpp
    shared/dto_model_to_view/toolbuttondto.h
    shared/dto_view_to_model/boxunloadrequesteddto.h
    shared/dto_view_to_model/toolbuttonactivateddto.h
    shared/dto_model_to_view/tooltextfielddto.h
    shared/dto_view_to_model/tooltextfieldactivateddto.h
    shared/dto_view_to_model/tooltextfieldcommitdto.h
    view_layer/tooltextfieldentrypanel.h view_layer/tooltextfieldentrypanel.cpp
    shared/dto_model_to_view/tooldropdowndto.h
    shared/dto_view_to_model/tooldropdownactivateddto.h
    view_layer/tooldropdownentrypanel.h view_layer/tooldropdownentrypanel.cpp
    control_layer/gdbcontrol.h control_layer/gdbcontrol.cpp











)

target_link_libraries(hornet PRIVATE
    Qt6::Widgets
    freetype
)

target_include_directories(hornet PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(hornet PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS hornet
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_finalize_executable(hornet)
