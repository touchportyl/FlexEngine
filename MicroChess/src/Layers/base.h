#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoShift
{

  class BaseLayer : public FlexEngine::Layer
  {
    const Path default_save_directory = Path::current("saves");
    const std::string default_save_name = "default";
    Path current_save_directory = default_save_directory;
    std::string current_save_name = default_save_name;

  public:
    BaseLayer() : Layer("Base Layer") {}
    ~BaseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
