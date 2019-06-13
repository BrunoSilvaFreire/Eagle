#include <eagle/Eagle.h>

#define EAGLE_EDITOR_NAME "Falcon"

int main() {
    Eagle::ApplicationCreateInfo editorInfo;
    editorInfo.appName = EAGLE_EDITOR_NAME;
    editorInfo.windowType = new Eagle::WindowGLFW(new Eagle::VulkanContext(), 1280, 720);
    editorInfo.coreLogLevel = Eagle::Log::TRACE;
    editorInfo.clientLogLevel = Eagle::Log::INFO;

    Eagle::Application editorApplication(editorInfo);
    editorApplication.run();
}