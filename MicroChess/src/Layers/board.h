#if 0
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace MicroChess
{

  class BoardLayer : public FlexEngine::Layer
  {
  public:
    BoardLayer() : Layer("Board Layer") {}
    ~BoardLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void SetupBoard();

    static float m_ScaleDebugTest;
    static Vector3 m_RotateDebugTest;
  };

}
#endif