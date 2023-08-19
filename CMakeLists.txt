cmake_minimum_required(VERSION 3.20)
project(webview_test)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(
        webview
        GIT_REPOSITORY https://github.com/webview/webview.git
        GIT_TAG master
)
FetchContent_GetProperties(webview)

if (NOT webview_POPULATED)
    # Library does not have a CMake build script
    # We have to do it ourselves
    FetchContent_Populate(webview)
    add_library(webview INTERFACE ${webview_SOURCE_DIR}/webview.cc)
    # target_sources(webview INTERFACE ${webview_SOURCE_DIR}/webview.h)
    target_include_directories(webview INTERFACE ${webview_SOURCE_DIR})
    # Set compile options
    # See: https://github.com/webview/webview/blob/master/script/build.sh
    if (WIN32)
        target_compile_definitions(webview INTERFACE WEBVIEW_EDGE)
        # See: https://github.com/webview/webview/blob/master/script/build.bat
        target_link_libraries(webview INTERFACE "-mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion")
        # target_link_libraries(webview INTERFACE "-mwindows -L./build/libs/webview2/build/native/x64 -lwebview -lWebView2Loader")
        target_include_directories(webview INTERFACE build/libs/webview2/build/native/include)
        # target_compile_options(...) ?
    elseif (APPLE)
        target_compile_definitions(webview INTERFACE WEBVIEW_COCOA)
        target_compile_definitions(webview INTERFACE "GUI_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
        target_compile_options(webview INTERFACE -Wno-all -Wno-extra -Wno-pedantic -Wno-delete-non-abstract-non-virtual-dtor)
        target_link_libraries(webview INTERFACE "-framework WebKit")
    elseif (UNIX)
        target_compile_definitions(webview INTERFACE WEBVIEW_GTK)
        target_compile_options(webview INTERFACE -Wall -Wextra -Wpedantic)
        target_link_libraries(webview INTERFACE "$(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)")
    endif ()
endif ()

set(HTML_PATH "${PROJECT_SOURCE_DIR}/html/index.html")
set(LOG_FILE_PATH "${PROJECT_SOURCE_DIR}/log_file.log")
add_executable(webview_hello webview_hello.cpp)
target_include_directories(webview_hello PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(webview_hello PRIVATE DEBUG=1)
configure_file(project.h.in project.h)
target_link_libraries(webview_hello PUBLIC webview dwmapi)
