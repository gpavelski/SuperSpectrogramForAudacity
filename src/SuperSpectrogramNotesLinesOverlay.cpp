/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramNotesLinesOverlay.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramNotesLinesOverlay.h"

#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/font.h>

//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
SuperSpectrogramNotesLinesOverlay::SuperSpectrogramNotesLinesOverlay(
   SuperSpectrogramConfig::NoteNaming style)
   : m_style(style)
{
   m_labels = MakeNoteLabels(
      style,
      SuperSpectrogramConstants::Notes::kMinNote,
      SuperSpectrogramConstants::Notes::kMaxNote
   );
}

//------------------------------------------------------------
// Enable / Disable
//------------------------------------------------------------
void SuperSpectrogramNotesLinesOverlay::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

//------------------------------------------------------------
// Rendering
//------------------------------------------------------------
void SuperSpectrogramNotesLinesOverlay::Render(
    wxDC& dc,
    const wxSize& size,
    const SuperSpectrogramDataAdapter& data,
    const SuperSpectrogramViewport& viewport
) const
{
    if (!m_enabled)
        return;

    if (data.GetNormalized().empty())
        return;

    dc.SetPen(wxPen(*wxWHITE, 1));
    dc.SetTextForeground(*wxWHITE);
    dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    double lastLabelY = -1e9;

    const auto& freqs = SuperSpectrogramConstants::Notes::kFrequencies;

    for (size_t i = 0; i < freqs.size(); ++i)
    {
        double y = FreqToWidgetY(freqs[i], size, data, viewport);
        if (y < 0)
            continue;

        // Line
        dc.DrawLine(0, static_cast<int>(y), size.GetWidth(), static_cast<int>(y));

        // Label
        const wxString& label = m_labels[i];

        wxCoord tw, th;
        dc.GetTextExtent(label, &tw, &th);

        double labelY = y - th - 2;

        if (labelY < lastLabelY + th + 4)
            continue;

        // Background
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(2, static_cast<int>(labelY - 1), tw + 6, th + 2);

        // Text
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawText(label, 5, static_cast<int>(labelY));

        lastLabelY = labelY;
    }
}

//------------------------------------------------------------
// Frequency -> Y mapping
//------------------------------------------------------------
double SuperSpectrogramNotesLinesOverlay::FreqToWidgetY(
    double freq,
    const wxSize& size,
    const SuperSpectrogramDataAdapter& data,
    const SuperSpectrogramViewport& viewport
) const
{
    const double fNyq = data.GetMaxFreq();
    const int rows = static_cast<int>(data.Rows());

    if (freq < 0.0 || freq > fNyq)
        return -1;

    const double binIndex = (freq / fNyq) * rows + 0.5;

    if (binIndex < viewport.Top() || binIndex > viewport.Bottom())
        return -1;

    const double rel =
        (binIndex - viewport.Top()) /
        (viewport.Bottom() - viewport.Top());

    return rel * size.GetHeight();
}

void SuperSpectrogramNotesLinesOverlay::SetNoteNamingStyle(
   SuperSpectrogramConfig::NoteNaming style)
{
   if (m_style == style)
      return;

   m_style = style;

   m_labels = MakeNoteLabels(
      style,
      SuperSpectrogramConstants::Notes::kMinNote,
      SuperSpectrogramConstants::Notes::kMaxNote
   );
}

std::vector<wxString>
SuperSpectrogramNotesLinesOverlay::MakeNoteLabels(
   SuperSpectrogramConfig::NoteNaming style,
   int minNote,
   int maxNote
) const
{
   const auto* names = &SuperSpectrogramConstants::Notes::kMixedNames;

   switch (style)
   {
   case SuperSpectrogramConfig::NoteNaming::Sharps:
      names = &SuperSpectrogramConstants::Notes::kSharpNames; break;
   case SuperSpectrogramConfig::NoteNaming::Flats:
      names = &SuperSpectrogramConstants::Notes::kFlatNames; break;
   case SuperSpectrogramConfig::NoteNaming::Mixed:
      names = &SuperSpectrogramConstants::Notes::kMixedNames; break;
   }

   std::vector<wxString> labels;
   labels.reserve(maxNote - minNote + 2);

   labels.emplace_back("sil");

   for (int note = minNote; note <= maxNote; ++note)
   {
      int pitch = note % 12;
      int octave = note / 12;

      labels.emplace_back(wxString::Format("%s%d", (*names)[pitch], octave));
   }

   return labels;
}
