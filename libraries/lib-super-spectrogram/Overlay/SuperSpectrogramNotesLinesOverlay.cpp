/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramNotesLinesOverlay.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Overlay/SuperSpectrogramNotesLinesOverlay.h"

//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
SuperSpectrogramNotesLinesOverlay::SuperSpectrogramNotesLinesOverlay(
   SuperSpectrogramConfig::NoteNaming style)
   : m_style(style)
{
   RebuildLabels(style);
}

//------------------------------------------------------------
// Enable / Disable
//------------------------------------------------------------
void SuperSpectrogramNotesLinesOverlay::SetEnabled(bool enabled)
{
   m_enabled = enabled;
}

//------------------------------------------------------------
// Update naming strategy
//------------------------------------------------------------
void SuperSpectrogramNotesLinesOverlay::SetNoteNamingStyle(
   SuperSpectrogramConfig::NoteNaming style)
{
   if (m_style == style)
      return;

   m_style = style;
   RebuildLabels(style);
}

//------------------------------------------------------------
// Label generation (state update only)
//------------------------------------------------------------
void SuperSpectrogramNotesLinesOverlay::RebuildLabels(
   SuperSpectrogramConfig::NoteNaming style)
{
   const auto* names = &SuperSpectrogramConstants::Notes::kMixedNames;

   switch (style)
   {
   case SuperSpectrogramConfig::NoteNaming::Sharps:
      names = &SuperSpectrogramConstants::Notes::kSharpNames;
      break;

   case SuperSpectrogramConfig::NoteNaming::Flats:
      names = &SuperSpectrogramConstants::Notes::kFlatNames;
      break;

   case SuperSpectrogramConfig::NoteNaming::Mixed:
      names = &SuperSpectrogramConstants::Notes::kMixedNames;
      break;
   }

   m_labels.clear();
   m_labels.reserve(
      SuperSpectrogramConstants::Notes::kMaxNote -
      SuperSpectrogramConstants::Notes::kMinNote + 2
   );

   m_labels.emplace_back("sil");

   for (int note = SuperSpectrogramConstants::Notes::kMinNote;
      note <= SuperSpectrogramConstants::Notes::kMaxNote;
      ++note)
   {
      int pitch = note % 12;
      int octave = note / 12;

      m_labels.emplace_back(
         wxString::Format("%s%d", (*names)[pitch], octave)
      );
   }
}

//------------------------------------------------------------
// Frequency conversion
//------------------------------------------------------------
double SuperSpectrogramNotesLinesOverlay::FreqToWidgetY(
   double freq,
   const wxSize& size,
   const SuperSpectrogramDataAdapter& data,
   const SuperSpectrogramViewport& viewport
) const
{
   const double fNyq = data.GetAnalysisMaxFreq();
   const int rows = static_cast<int>(data.Rows());

   if (freq < 0.0 || freq > fNyq)
      return -1;

   // Matrix already inverted:
   // high frequencies -> small row indices
   // low frequencies  -> large row indices
   const double binIndex =
      ((fNyq - freq) / fNyq) * (rows - 1);

   if (binIndex < viewport.Top() ||
      binIndex > viewport.Bottom())
   {
      return -1;
   }

   const double rel =
      (binIndex - viewport.Top()) /
      (viewport.Bottom() - viewport.Top());

   return rel * size.GetHeight();
}

//------------------------------------------------------------
// Overlay Rendering
//------------------------------------------------------------
void SuperSpectrogramNotesLinesOverlay::Render(
   wxDC& dc,
   const wxSize& size,
   const SuperSpectrogramDataAdapter& data,
   const SuperSpectrogramViewport& viewport
) const
{
   if (!m_enabled || data.GetNormalized().empty())
      return;

   dc.SetPen(wxPen(*wxWHITE, 1));
   dc.SetTextForeground(*wxWHITE);
   dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT,
      wxFONTSTYLE_NORMAL,
      wxFONTWEIGHT_NORMAL));

   const auto& freqs = SuperSpectrogramConstants::Notes::kFrequencies;

   double lastLabelY = -1e9;

   for (int i = static_cast<int>(freqs.size()) - 1;
      i >= 0;
      --i)
   {
      double y = FreqToWidgetY(freqs[i], size, data, viewport);
      if (y < 0)
         continue;

      dc.DrawLine(0, (int)y, size.GetWidth(), (int)y);

      const wxString& label = m_labels[i];

      wxCoord tw, th;
      dc.GetTextExtent(label, &tw, &th);

      double labelY = y - th - 2;

      if (labelY < lastLabelY + th + 4)
         continue;

      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(2, (int)(labelY - 1), tw + 6, th + 2);

      dc.SetPen(*wxWHITE_PEN);
      dc.DrawText(label, 5, (int)labelY);

      lastLabelY = labelY;
   }
}
