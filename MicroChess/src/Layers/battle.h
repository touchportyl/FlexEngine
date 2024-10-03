#pragma once
#include <FlexEngine.h>

#include "BattleSystems/battlesystem.h"
#include "CharacterMoves/moves.h"
using namespace FlexEngine;

namespace ChronoShift
{
  class BattleLayer : public FlexEngine::Layer
  {
  public:
    BattleLayer() : Layer("Battle Layer") {}
    ~BattleLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void SetupBattle();

    BattleSystem m_battlesystem;
  };



}
