/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramInteractionController.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_INTERACTION_CONTROLLER__
#define __SUPER_SPECTROGRAM_INTERACTION_CONTROLLER__

#include <wx/event.h>
#include <wx/gdicmn.h>
#include "SuperSpectrogramMouseEvent.h"

class SuperSpectrogramViewport;

class SuperSpectrogramInteractionController
{
public:
   bool OnMouse(
      const SuperSpectrogramMouseEvent& event,
      SuperSpectrogramViewport& viewport,
      const wxSize& size
   );

   bool OnWheel(
      const SuperSpectrogramMouseEvent& event,
      SuperSpectrogramViewport& viewport,
      const wxSize& size
   );

private:
   wxPoint m_lastMouse;
};

#endif
