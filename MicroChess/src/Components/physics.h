#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoShift
{

  class BoundingBox2D
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 size = Vector2::One;
    Vector2 min;
    Vector2 max;
  };

  class OnHover
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_hovering;
    bool on_enter;
    bool on_exit;
  };

  class OnClick
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_clicked;
  };

  class AABB
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 min;
    Vector2 max;
  };

  class Rigidbody 
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 velocity;
    bool is_static;
  };
}
