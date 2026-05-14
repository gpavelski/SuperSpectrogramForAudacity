/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramTimeTicksOverlay.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_TIME_TICKS_OVERLAY__
#define __SUPER_SPECTROGRAM_TIME_TICKS_OVERLAY__

#include <wx/dc.h>
#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramOverlay.h"

// Forward declarations
class SuperSpectrogramDataAdapter;
class SuperSpectrogramViewport;

class SuperSpectrogramTimeTicksOverlay : public ISpectrogramOverlay
{
public:
   explicit SuperSpectrogramTimeTicksOverlay(SuperSpectrogramConfig::TimeTickMode mode);

   void SetMode(SuperSpectrogramConfig::TimeTickMode mode);

   SuperSpectrogramConfig::TimeTickMode GetMode() const {
      return m_mode;
   };

   void Render(
      wxDC& dc,
      const wxSize& size,
      const SuperSpectrogramDataAdapter& data,
      const SuperSpectrogramViewport& viewport
   ) const override;

   void SetEnabled(bool enabled) override;
   bool IsEnabled() const override { return m_enabled; }

private:
   void DrawTickLabel(
      wxDC& dc,
      const wxSize& size,
      double fx,
      const wxString& label
   ) const;

private:
   SuperSpectrogramConfig::TimeTickMode m_mode;
   bool m_enabled{ true };
};

#endif
