#include <FlexEngine.h>
#include <FlexEngine/Core/EntryPoint.h>

#include "States.h"

namespace ChronoShift
{
  class FlexApplication : public FlexEngine::Application
  {
  public:
    FlexApplication()
    {
      FlexEngine::ApplicationStateManager::SetState<MainState>();
    }
  };

}

namespace FlexEngine
{
  Application* CreateApplication()
  {
    return new ChronoShift::FlexApplication();
  }
}
