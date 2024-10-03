#pragma once
#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoShift
{
  class IsPlayer
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_player = true;
  };

  class CharacterName
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex character_name;
  };

  class CharacterRole
  {
    FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex character_name;
  };

  class CharacterHealth
  { FLX_REFL_SERIALIZABLE
  public:
    int base_health;
    int current_health;
  };

  class CharacterSpeed
  { FLX_REFL_SERIALIZABLE
  public:
    int base_speed;
    int current_speed;
  };

  class CharacterWeapon
  { FLX_REFL_SERIALIZABLE
  public:
    // Weapon
    FlexECS::EntityID equipped_weapon;
  };

  class CharacterChronoGear
  { FLX_REFL_SERIALIZABLE
  public:
    // Chrono Gear
    FlexECS::EntityID equipped_chrono_gear;
  };

  class CharacterPassive
  { FLX_REFL_SERIALIZABLE
  public:
    // Passive
    FlexECS::EntityID character_passive;
  };
}