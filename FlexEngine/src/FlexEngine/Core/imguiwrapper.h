#pragma once

#include "flx_api.h"

#include "layer.h" // <string>

#include <imgui.h>
#include <imgui_internal.h>

#include "window.h"

namespace FlexEngine
{

  class __FLX_API ImGuiWrapper
  {
  public:

    // Sets the ImGui context for the current window
    // ImGui is not recommended to be used in DLLs because of the global context pointer
    // We must call this function before using ImGui across any DLL boundaries
    #define AlignImGuiContext(window) ImGui::SetCurrentContext(window->GetImGuiContext());

    static ImGuiContext* Init(Window* window);
    static void Shutdown(ImGuiContext* imgui_context);

    static void BeginFrame();
    static void EndFrame();

    static unsigned int GetActiveWidgetID();

  };

}