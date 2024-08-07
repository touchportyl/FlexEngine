#pragma once

#include <string>

namespace FlexEngine
{

  // abstract class Layer
  // build upon this class to create your own layers
  class Layer
  {
    std::string m_layer_name;

  public:
    Layer(const std::string& name = "Layer") : m_layer_name(name) {}
    virtual ~Layer() = default;

    // setter

    void SetName(const std::string& name) { m_layer_name = name; }

    // getter

    const std::string& GetName() const { return m_layer_name; }

    // called once when layer is attached to the layer stack
    virtual void OnAttach() = 0;

    // called once when layer is detached from the layer stack
    virtual void OnDetach() = 0;

    // automatically updated by the layer stack
    virtual void Update() = 0;
  };

}