#include "rendering.h"

namespace MicroChess
{

  FLX_REFL_REGISTER_START(IsActive)
    FLX_REFL_REGISTER_PROPERTY(is_active)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Parent)
    FLX_REFL_REGISTER_PROPERTY(parent)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Position)
    FLX_REFL_REGISTER_PROPERTY(position)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Scale)
    FLX_REFL_REGISTER_PROPERTY(scale)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Rotation)
    FLX_REFL_REGISTER_PROPERTY(rotation)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(ZIndex)
    FLX_REFL_REGISTER_PROPERTY(z)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Shader)
    FLX_REFL_REGISTER_PROPERTY(shader)
  FLX_REFL_REGISTER_END;
  
  FLX_REFL_REGISTER_START(Sprite)
    FLX_REFL_REGISTER_PROPERTY(texture)
    FLX_REFL_REGISTER_PROPERTY(color)
    FLX_REFL_REGISTER_PROPERTY(color_to_add)
    FLX_REFL_REGISTER_PROPERTY(color_to_multiply)
    FLX_REFL_REGISTER_PROPERTY(alignment)
  FLX_REFL_REGISTER_END;

}