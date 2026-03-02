cmake_minimum_required(VERSION 3.5)
project(hornet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    model_layer/modelaccess.cpp
    view_layer/window.cpp
    control_layer/control.cpp
    view_layer/theme.cpp
    service_layer/projectservice.cpp
    service_layer/validator.cpp
    model_layer/numbermodel.cpp
    service_layer/numberservice.cpp
    service_layer/diskreaderservice.cpp
    service_layer/diskwriterservice.cpp
    view_layer/titlebar.cpp
    view_layer/resizehandle.cpp
    view_layer/lowerwidget.cpp
    view_layer/overlaywidget.cpp
    view_layer/view.cpp
)

set(PROJECT_HEADERS
    model_layer/modelaccess.h
    view_layer/window.h
    control_layer/control.h
    service_layer/projectservice.h
    service_layer/validator.h
    view_layer/theme.h
    model_layer/numbermodel.h
    model_layer/imodelaccess_read.h
    model_layer/imodelaccess_readwrite.h
    shared/dto/numberdto.h
    service_layer/numberservice.h
    service_layer/diskreaderservice.h
    service_layer/diskwriterservice.h
    view_layer/titlebar.h
    view_layer/resizehandle.h
    view_layer/lowerwidget.h
    view_layer/overlaywidget.h
    view_layer/view.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(hornet
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
else()
    if(ANDROID)
        add_library(hornet SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
    else()
        add_executable(hornet
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
    endif()
endif()

target_link_libraries(hornet PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(hornet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.hornet)
endif()

set_target_properties(hornet PROPERTIES
    ${BUNDLE_ID_OPTION}
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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(hornet)
endif()
