#pragma once

#include "raylib.h"

class Input
{
public:
  static Vector2 GetPointerPosition()
  {
    if(GetTouchPointCount() > 0)
      return GetTouchPosition(0);
    return GetMousePosition();
  }

  static bool IsPointerDown()
  {
    return GetTouchPointCount() > 0 || IsMouseButtonDown(MOUSE_LEFT_BUTTON);
  }
};