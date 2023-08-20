cmake_minimum_required(VERSION 3.20)
project(webview_test)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/Michaelzhouisnotwhite/webview-cmake.git
    GIT_TAG main
)
FetchContent_MakeAvailable(webview)

set(HTML_PATH "${PROJECT_SOURCE_DIR}/html/index.html")
set(LOG_FILE_PATH "${PROJECT_SOURCE_DIR}/log_file.log")
add_executable(webview_hello webview_hello.cpp)
target_include_directories(webview_hello PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(webview_hello PRIVATE DEBUG=1)
configure_file(project.h.in project.h)

target_link_libraries(webview_hello PUBLIC webview dwmapi)
if(WIN32)
    target_link_libraries(webview_hello PUBLIC dwmapi)
endif()
