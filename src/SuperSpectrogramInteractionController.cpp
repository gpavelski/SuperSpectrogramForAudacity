/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramInteractionController.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramInteractionController.h"
#include "SuperSpectrogramViewport.h"

//------------------------------------------------------------
// Mouse handling (pan)
//------------------------------------------------------------
bool SuperSpectrogramInteractionController::OnMouse(
   const SuperSpectrogramMouseEvent& event,
   SuperSpectrogramViewport& viewport,
   const wxSize& size
)
{
   if (event.leftDown)
   {
      m_lastMouse = wxPoint(event.x, event.y);
      return false;
   }
   else if (event.dragging)
   {
      wxPoint pos(event.x, event.y);
      wxPoint delta = pos - m_lastMouse;
      m_lastMouse = pos;

      viewport.Pan(delta.x, delta.y, size);
      return true;
   }

   return false;
}

//------------------------------------------------------------
// Wheel handling (zoom)
//------------------------------------------------------------
bool SuperSpectrogramInteractionController::OnWheel(
   const SuperSpectrogramMouseEvent& event,
   SuperSpectrogramViewport& viewport,
   const wxSize& size
)
{
   if (event.wheelRotation == 0)
      return false;

   double factor = (event.wheelRotation > 0) ? 0.8 : 1.25;

   double fx = static_cast<double>(event.x) / size.GetWidth();
   double fy = static_cast<double>(event.y) / size.GetHeight();

   viewport.Zoom(factor, fx, fy);

   return true;
}

