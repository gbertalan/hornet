cmake_minimum_required(VERSION 3.5)
project(hornet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

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
    view_layer/customscrollbar.h view_layer/customscrollbar.cpp
)

target_link_libraries(hornet PRIVATE Qt6::Widgets)
target_include_directories(hornet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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
