#include "GuiWindow.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
GuiWindow::GuiWindow(int width, int height, const std::string& title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window_ = glfwCreateWindow(
        width,
        height,
        title.c_str(),
        nullptr,
        nullptr
    );

    if (window_ == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
}

GuiWindow::~GuiWindow()
{
    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    glfwTerminate();
}

bool GuiWindow::shouldClose() const
{
    return glfwWindowShouldClose(window_);
}

void GuiWindow::pollEvents() const
{
    glfwPollEvents();
}

void GuiWindow::swapBuffers() const
{
    glfwSwapBuffers(window_);
}

void GuiWindow::beginFrame(float red, float green, float blue, float alpha) const
{
    int displayWidth = 0;
    int displayHeight = 0;

    glfwGetFramebufferSize(window_, &displayWidth, &displayHeight);

    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

GLFWwindow* GuiWindow::nativeHandle() const
{
    return window_;
}