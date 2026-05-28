# CMakeList.txt: проект CMake для bank_signal; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (bank_signal "bank_signal.cpp" "bank_signal.h")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bank_signal PROPERTY CXX_STANDARD 20)
endif()



include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# -----------------------------
# GLFW
# -----------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
)
FetchContent_MakeAvailable(glfw)
# -----------------------------
# Dear ImGui
# -----------------------------
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.9b.tar.gz
)
FetchContent_MakeAvailable(imgui)

# -----------------------------
# OpenGL
# -----------------------------
find_package(OpenGL REQUIRED)


target_include_directories(bank_signal PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(bank_signal PRIVATE
    nlohmann_json::nlohmann_json
    glfw
    OpenGL::GL
)

##if (WIN32)
  ##  target_link_libraries(bank_queue PRIVATE ws2_32)
##endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
