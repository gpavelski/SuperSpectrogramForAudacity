/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramNotesLinesOverlay.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

#include <vector>
#include <wx/brush.h>
#include <wx/dc.h>
#include <wx/pen.h>
#include <wx/font.h>
#include <wx/string.h>

#include "Config/SuperSpectrogramConfig.h"
#include "Config/SuperSpectrogramConstants.h"
#include "Overlay/SuperSpectrogramOverlay.h"
#include "DataAdapter/SuperSpectrogramDataAdapter.h"
#include "View/SuperSpectrogramViewport.h"
#include "Config/SuperSpectrogramConstants.h"

class SuperSpectrogramDataAdapter;
class SuperSpectrogramViewport;

class SuperSpectrogramNotesLinesOverlay : public ISpectrogramOverlay
{
public:
   explicit SuperSpectrogramNotesLinesOverlay(
      SuperSpectrogramConfig::NoteNaming style);

   void SetEnabled(bool enabled) override;
   bool IsEnabled() const override { return m_enabled; }

   void SetNoteNamingStyle(SuperSpectrogramConfig::NoteNaming style);

   void Render(
      wxDC& dc,
      const wxSize& size,
      const SuperSpectrogramDataAdapter& data,
      const SuperSpectrogramViewport& viewport
   ) const override;

private:
   // Rebuilds label cache when naming style changes
   void RebuildLabels(SuperSpectrogramConfig::NoteNaming style);

   // Converts frequency domain value into widget Y coordinate
   double FreqToWidgetY(
      double freq,
      const wxSize& size,
      const SuperSpectrogramDataAdapter& data,
      const SuperSpectrogramViewport& viewport
   ) const;

private:
   std::vector<wxString> m_labels;

   SuperSpectrogramConfig::NoteNaming m_style;
   bool m_enabled{ true };
};

