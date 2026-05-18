/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramMouseEvent.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

struct SuperSpectrogramMouseEvent
{
   int x{ 0 };
   int y{ 0 };

   bool leftDown{ false };
   bool leftUp{ false };
   bool dragging{ false };

   int wheelRotation{ 0 };
};

