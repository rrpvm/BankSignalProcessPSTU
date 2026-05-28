#include <string>
struct GLFWwindow;

class GuiWindow final
{
public:
    GuiWindow(int width, int height, const std::string& title);
    ~GuiWindow();

    GuiWindow(const GuiWindow&) = delete;
    GuiWindow& operator=(const GuiWindow&) = delete;

    GuiWindow(GuiWindow&&) = delete;
    GuiWindow& operator=(GuiWindow&&) = delete;

    bool shouldClose() const;
    void pollEvents() const;
    void swapBuffers() const;

    void beginFrame(float red, float green, float blue, float alpha) const;

    GLFWwindow* nativeHandle() const;

private:
    GLFWwindow* window_ = nullptr;
};