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

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

set(PROJECT_SOURCES
    main.cpp
    model_layer/modelaccess.cpp
    model_layer/windowmodel.cpp
    model_layer/numbermodel.cpp
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
    service_layer/projectservice.cpp
    service_layer/validator.cpp
    service_layer/numberservice.cpp
    service_layer/windowservice.cpp
    service_layer/diskreaderservice.cpp
    service_layer/diskwriterservice.cpp
    resources/resources.qrc
)
set(PROJECT_HEADERS
    model_layer/modelaccess.h
    model_layer/windowmodel.h
    model_layer/numbermodel.h
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
    service_layer/projectservice.h
    service_layer/validator.h
    service_layer/numberservice.h
    service_layer/windowservice.h
    service_layer/diskreaderservice.h
    service_layer/diskwriterservice.h
    shared/dto/numberdto.h
    shared/dto/windowdto.h
)
qt_add_executable(hornet
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    view_layer/editor/editor.h view_layer/editor/editor.cpp
    shared/dto/editorsettingsdto.h
    shared/dto/editortextcontentsdto.h
    shared/dto/editorvisiblelinesdto.h





)

target_link_libraries(hornet PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
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
