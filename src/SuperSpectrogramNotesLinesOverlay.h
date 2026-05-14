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
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramOverlay.h"
#include "SuperSpectrogramViewport.h"

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
   std::vector<wxString> MakeNoteLabels(
      SuperSpectrogramConfig::NoteNaming style,
      int minNote,
      int maxNote
   ) const;

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

#endif // __SUPER_SPECTROGRAM_NOTES_LINES_OVERLAY__
