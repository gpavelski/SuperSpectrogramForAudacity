/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramTimeTicksOverlay.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

#include <wx/dc.h>

#include "Config/SuperSpectrogramConfig.h"
#include "DataAdapter/SuperSpectrogramDataAdapter.h"
#include "Overlay/SuperSpectrogramOverlay.h"
#include "View/SuperSpectrogramViewport.h"

#include <cmath>

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

