set(projectName acanthis)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Qml Quick Gui Gui OpenGL REQUIRED)

add_compile_definitions(_ACANTHIS)

set(acanthis_HDRS
    coreapplication.h
    mainwindowfactory.h
    settingsimpl.h
    settings.h
    mainwindow.h
    graphdocument.h
    interfaceversion.h
)

set(acanthis_SRCS
    main.cpp
    coreapplication.cpp
    mainwindowfactory.cpp
    settingsimpl.cpp
    mainwindow.cpp
    graphdocument.cpp
)

qt5_wrap_ui(acanthis_UI
   # UI/aboutdlg.ui
   UI/licenseagreement.ui
)

qt5_add_resources(acanthis_RSRC resource.qrc dialogs/settings/settingsdialog.qrc)

set(projectFiles ${acanthis_HDRS} ${acanthis_SRCS} ${acanthis_UI} ${acanthis_RSRC})

if (WIN32)
        add_executable(${projectName} WIN32 ${projectFiles} icons.rc)
        if(MINGW)
            # https://stackoverflow.com/a/18138926
            # MinGW compilation does not bring in the required DLLs to make the executable self-contained
            # The missing DLLs are:
            # - libstdc++-6.dll
            # - libwinpthread-1.dll
            # - libgcc_s_seh-1.dll
            # Additionally, windeployqt does not drag them into the build directory, so it might be
            # necessary to manually find and copy them after compilation
            set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        endif(MINGW)
endif(WIN32)
if(UNIX AND NOT APPLE)
        add_executable(${projectName} ${projectFiles})
        if (Qt5_POSITION_INDEPENDENT_CODE)
            SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
        endif()
endif()
if(APPLE)
        set(MACOSX_BUNDLE_ICON_FILE acanthis.icns)
        set(acanthis_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/acanthis.icns)
        set_source_files_properties(${acanthis_ICON} PROPERTIES
               MACOSX_PACKAGE_LOCATION "Resources")
        set(graph_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/graph.icns)
        set_source_files_properties(${graph_ICON} PROPERTIES
               MACOSX_PACKAGE_LOCATION "Resources")
        add_definitions(-DGL_SILENCE_DEPRECATION)
        add_executable(${projectName} MACOSX_BUNDLE ${acanthis_ICON} ${graph_ICON}
            ${projectFiles})
        set_target_properties(${projectName} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist)
endif(APPLE)

find_package(OpenGL REQUIRED)

set(modules_gui "" CACHE INTERNAL "modules_gui" FORCE)
set(MODULES_GUI_ACANTHIS TRUE)
set(MODULES_GUI FALSE)
set(MODULES_CLI FALSE)
set(MODULES_CLI_TEST FALSE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST FALSE)
add_subdirectory(../modules modules)

target_link_libraries(${projectName} salalib genlib mgraph440 Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::OpenGL
    OpenGL::GL OpenGL::GLU ${modules_gui} ${modules_core})

add_subdirectory(dialogs)
