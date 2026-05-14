/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramNotesLinesOverlay.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_NOTES_LINES_OVERLAY__
#define __SUPER_SPECTROGRAM_NOTES_LINES_OVERLAY__

#include <vector>
#include <wx/string.h>
#include <wx/dc.h>

#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramConstants.h"
#include "SuperSpectrogramOverlay.h"

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

#endif
