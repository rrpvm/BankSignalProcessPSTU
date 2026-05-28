#pragma once

struct GLFWwindow;

class ImGuiLayer final
{
public:
    explicit ImGuiLayer(GLFWwindow* window);
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    ImGuiLayer(ImGuiLayer&&) = delete;
    ImGuiLayer& operator=(ImGuiLayer&&) = delete;

    void beginFrame() const;
    void endFrame() const;

private:
    static constexpr const char* GlSlVersion = "#version 130";
};