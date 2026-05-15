/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramInteractionController.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_INTERACTION_CONTROLLER__
#define __SUPER_SPECTROGRAM_INTERACTION_CONTROLLER__

#include <wx/gdicmn.h>

#include "View/SuperSpectrogramMouseEvent.h"
#include "View/SuperSpectrogramViewport.h"

class SuperSpectrogramViewport;

class SuperSpectrogramInteractionController
{
public:
   // Processes mouse drag / press / release events and updates viewport
   bool OnMouse(
      const SuperSpectrogramMouseEvent& event,
      SuperSpectrogramViewport& viewport,
      const wxSize& size);

   // Processes zoom gesture from wheel input
   bool OnWheel(
      const SuperSpectrogramMouseEvent& event,
      SuperSpectrogramViewport& viewport,
      const wxSize& size);

private:
   wxPoint m_lastMouse{ 0, 0 };
};

#endif
