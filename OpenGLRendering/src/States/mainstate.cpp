#include "mainstate.h"

#include "States.h"
#include "Layers.h"

namespace OpenGLRendering
{

  void MainState::OnEnter()
  {
    window = Application::OpenWindow(
      {
        "FlexEngine - OpenGL Rendering",
        1280, 720 + 30, // 30 pixels for the title bar
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, false },
          { GLFW_RESIZABLE, false }
        }
      }
    );
    AlignImGuiContext(window);

    // load audio handler

    // load assets only after the window has been created
    AssetManager::Load();
    FreeQueue::Push(std::bind(&AssetManager::Unload), "OpenGLRendering AssetManager");

    window->SetTargetFPS();
    window->SetVSync(false);
    window->SetIcon(FLX_ASSET_GET(Asset::Texture, R"(\images\flexengine\flexengine_icon_white.png)"));
    //window->SetIcon(FLX_ASSET_GET(Asset::Texture, R"(\images\flexengine\flexengine-256.png)"));
    window->PushLayer(std::make_shared<MainLayer>());
  }

  void MainState::OnExit()
  {
    // Free in reverse add order...
    FreeQueue::RemoveAndExecute("OpenGLRendering AssetManager");

    Application::CloseWindow(window);
    window.reset();
  }

  void MainState::Update()
  {
    window->Update();
  }

}