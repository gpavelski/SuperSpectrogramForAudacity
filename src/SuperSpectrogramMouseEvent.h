/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramMouseEvent.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_MOUSE_EVENT__
#define __SUPER_SPECTROGRAM_MOUSE_EVENT__

struct SuperSpectrogramMouseEvent
{
   int x{0};
   int y{0};

   bool leftDown{false};
   bool leftUp{false};
   bool dragging{false};

   int wheelRotation{0};
};

#endif