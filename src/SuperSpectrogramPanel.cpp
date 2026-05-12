/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramPanel.cpp

  Guilherme Pavelski

*******************************************************************//**

\class SuperSpectrogramPanel
\brief Responsible for UI interaction of the Super Spectrogram

wxPanel responsible for rendering a spectrogram bitmap, handling
user interaction (pan, zoom, reset), and drawing auxiliary overlays
such as note lines and time ticks.

*//*******************************************************************/

#include "SuperSpectrogramPanel.h"
#include "SuperSpectrogramColormapFactory.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <algorithm>
#include <cmath>

wxBEGIN_EVENT_TABLE(SuperSpectrogramPanel, wxPanel)
EVT_PAINT(SuperSpectrogramPanel::OnPaint)
EVT_SIZE(SuperSpectrogramPanel::OnSize)
EVT_LEFT_DOWN(SuperSpectrogramPanel::OnMouse)
EVT_LEFT_UP(SuperSpectrogramPanel::OnMouse)
EVT_MOTION(SuperSpectrogramPanel::OnMouse)
EVT_RIGHT_DOWN(SuperSpectrogramPanel::OnRightClick)
EVT_MOUSEWHEEL(SuperSpectrogramPanel::OnWheel)
wxEND_EVENT_TABLE()

//----------------------------------------------------------------------
// Constructs the spectrogram panel and initializes rendering defaults
//----------------------------------------------------------------------
SuperSpectrogramPanel::SuperSpectrogramPanel(wxWindow* parent)
   : wxPanel(parent), m_showNoteLines(false)
{
   SetBackgroundStyle(wxBG_STYLE_PAINT);
   SetDoubleBuffered(true);
   m_colormap = SuperSpectrogramColormapFactory::Create(
      SuperSpectrogramConfig::Colormap::Jet);
   mNoteLabels = MakeNoteLabels(mNoteNamingStyle, SuperSpectrogramConstants::Notes::kMinNote, SuperSpectrogramConstants::Notes::kMaxNote);
}

void SuperSpectrogramPanel::SetColormap(
   SuperSpectrogramConfig::Colormap type)
{
   m_colormap = SuperSpectrogramColormapFactory::Create(type);

   Refresh();
}

//----------------------------------------------------------------------
// Sets the spectrogram data matrix and associated metadata, then
// rebuilds the backing bitmap and resets the view extents
//----------------------------------------------------------------------
void SuperSpectrogramPanel::SetData(const SuperSpectrogramFrame& frame)
{
   m_data.SetFrame(frame);

   m_viewport.SetBounds(
      m_data.Rows(),
      m_data.Cols()
   );

   Refresh();
}

std::vector<wxString> SuperSpectrogramPanel::MakeNoteLabels(
   SuperSpectrogramConfig::NoteNaming style,
   int minNote,
   int maxNote)
{

   const auto* names = &SuperSpectrogramConstants::Notes::kMixedNames;

   switch (style)
   {
   case SuperSpectrogramConfig::NoteNaming::Sharps: names = &SuperSpectrogramConstants::Notes::kSharpNames; break;
   case SuperSpectrogramConfig::NoteNaming::Flats:  names = &SuperSpectrogramConstants::Notes::kFlatNames;  break;
   case SuperSpectrogramConfig::NoteNaming::Mixed:  names = &SuperSpectrogramConstants::Notes::kMixedNames; break;
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

void SuperSpectrogramPanel::SetNoteNamingStyle(SuperSpectrogramConfig::NoteNaming style)
{
   if (mNoteNamingStyle == style)
      return;

   mNoteNamingStyle = style;

   mNoteLabels = MakeNoteLabels(style, SuperSpectrogramConstants::Notes::kMinNote, SuperSpectrogramConstants::Notes::kMaxNote);

   Refresh();
}

//----------------------------------------------------------------------
// Handles paint events by rendering the current view to the panel
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnPaint(wxPaintEvent&)
{
   wxAutoBufferedPaintDC dc(this);
   Render(dc, GetClientSize());
}

//----------------------------------------------------------------------
// Handles resize events by triggering a repaint when data is present
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnSize(wxSizeEvent& event)
{
   if (!m_data.GetNormalized().empty()) {
      Refresh();
   }
   event.Skip();
}

//----------------------------------------------------------------------
// Renders horizontal musical note reference lines and labels
//----------------------------------------------------------------------
void SuperSpectrogramPanel::DrawNoteLines(wxDC& dc, const wxSize& size) const
{
   if (m_data.GetNormalized().empty()) return;

   dc.SetPen(wxPen(*wxWHITE, 1));
   dc.SetTextForeground(*wxWHITE);
   dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

   double lastLabelY = -1e9;

   for (size_t i = 0; i < SuperSpectrogramConstants::Notes::kFrequencies.size(); ++i) {
      double freq = SuperSpectrogramConstants::Notes::kFrequencies[i];
      double y = FreqToWidgetY(freq, size.GetHeight());
      if (y < 0) continue;

      // Draw the horizontal line across full widget width
      dc.DrawLine(0, y, size.GetWidth(), y);

      // Compute label position
      const wxString& label = mNoteLabels[i];
      wxCoord tw, th;
      dc.GetTextExtent(label, &tw, &th);
      double labelY = y - th - 2;

      if (labelY < lastLabelY + th + 4)
         continue; // avoid overlaps

      // Draw black background for readability
      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(2, labelY - 1, tw + 6, th + 2);

      // Draw text
      dc.SetPen(*wxWHITE_PEN);
      dc.DrawText(label, 5, labelY);

      lastLabelY = labelY;
   }
}

//----------------------------------------------------------------------
// Converts a frequency value (Hz) to a Y coordinate in widget space,
// accounting for the current vertical view range
//----------------------------------------------------------------------
double SuperSpectrogramPanel::FreqToWidgetY(double freq, int widgetHeight) const
{
   const int rows = static_cast<int>(m_data.Rows());
   const double fNyq = m_data.GetMaxFreq();

   if (freq < 0.0 || freq > fNyq)
      return -1;

   // Frequency -> bin center
   const double binIndex = (freq / fNyq) * rows + 0.5;

   // Clip to valid bin range
   if (binIndex < m_viewport.Top() || binIndex > m_viewport.Bottom())
      return -1;

   // Map visible bins -> widget Y
   const double binRel =
      (binIndex - m_viewport.Top()) /
      (m_viewport.Bottom() - m_viewport.Top());

   return binRel * widgetHeight;
}

//----------------------------------------------------------------------
// Renders the spectrogram view (including overlays) into the target DC
//----------------------------------------------------------------------
void SuperSpectrogramPanel::Render(wxDC& dc, const wxSize& target) const
{
   dc.SetBackground(*wxBLACK_BRUSH);
   dc.Clear();

   if (m_data.GetNormalized().empty() || !m_colormap)
      return;

   const int width = target.GetWidth();
   const int height = target.GetHeight();

   wxImage img(width, height, false);
   unsigned char* data = img.GetData();

   // Viewport bounds
   const double left = m_viewport.Left();
   const double right = m_viewport.Right();
   const double top = m_viewport.Top();
   const double bottom = m_viewport.Bottom();

   const double dx = (right - left) / width;
   const double dy = (bottom - top) / height;

   const auto& normalized = m_data.GetNormalized();
   size_t cols = m_data.Cols();

   for (int py = 0; py < height; ++py)
   {
      for (int px = 0; px < width; ++px)
      {
         // Map screen -> data indices
         double fx = left + px * dx;
         double fy = top + py * dy;

         size_t ix = static_cast<size_t>(fx);
         size_t iy = static_cast<size_t>(fy);

         float norm = normalized[iy * cols + ix];

         const wxColour& c = m_colormap->Map(norm);

         size_t offset = 3 * (py * width + px);
         data[offset + 0] = c.Red();
         data[offset + 1] = c.Green();
         data[offset + 2] = c.Blue();
      }
   }

   wxBitmap bmp(img);
   dc.DrawBitmap(bmp, 0, 0);

   // Overlays stay unchanged
   if (m_showNoteLines)
      DrawNoteLines(dc, target);

   DrawTimeTicks(dc, target);
}

//----------------------------------------------------------------------
// Renders the currently visible spectrogram region into a bitmap,
// used primarily for exporting the view as an image
//----------------------------------------------------------------------
wxBitmap SuperSpectrogramPanel::RenderCurrentViewToBitmap() const
{
   wxSize size = GetClientSize();
   wxBitmap bmp(size.GetWidth(), size.GetHeight(), 24);

   wxMemoryDC dc(bmp);
   Render(dc, size);
   dc.SelectObject(wxNullBitmap);

   return bmp;
}

//----------------------------------------------------------------------
// Handles mouse wheel input to perform zooming centered at the cursor
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnWheel(wxMouseEvent& event)
{
   double factor = (event.GetWheelRotation() > 0) ? 0.8 : 1.25;

   wxSize size = GetClientSize();

   double fx = (double)event.GetX() / size.GetWidth();
   double fy = (double)event.GetY() / size.GetHeight();

   m_viewport.Zoom(factor, fx, fy);

   Refresh();
}

//----------------------------------------------------------------------
// Handles mouse press, release, and drag events to support panning
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnMouse(wxMouseEvent& event)
{
   if (event.LeftDown()) {
      m_lastMouse = event.GetPosition();
      CaptureMouse();
   }
   else if (event.LeftUp()) {
      if (HasCapture()) ReleaseMouse();
   }
   else if (event.Dragging() && event.LeftIsDown())
   {
      wxPoint pos = event.GetPosition();
      wxPoint delta = pos - m_lastMouse;
      m_lastMouse = pos;

      m_viewport.Pan(delta.x, delta.y, GetClientSize());

      Refresh();
   }
}

//----------------------------------------------------------------------
// Resets the view extents to show the entire spectrogram
//----------------------------------------------------------------------
void SuperSpectrogramPanel::ResetView()
{
   m_viewport.Reset();
   Refresh(false);
}

//----------------------------------------------------------------------
// Handles right-click events by resetting the spectrogram view
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnRightClick(wxMouseEvent& event)
{
   ResetView();
   event.Skip();
}

//----------------------------------------------------------------------
// Draws time tick labels along the bottom edge of the panel, selecting
// an appropriate tick spacing based on zoom level
//----------------------------------------------------------------------
void SuperSpectrogramPanel::DrawTimeTicks(wxDC& dc, const wxSize& size) const
{
   if (m_timeTickMode == SuperSpectrogramConfig::TimeTickMode::None ||
      m_data.GetNumSamples() == 0 ||
      m_data.GetNormalized().empty())
      return;

   dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
   dc.SetTextForeground(*wxWHITE);

   const size_t numFrames = m_data.Cols();

   const double viewLeftFrame = m_viewport.Left();
   const double viewRightFrame = m_viewport.Right();

   const double frameSpan = viewRightFrame - viewLeftFrame;
   if (frameSpan <= 0)
      return;

   // --------------------------------------
   // Mode: SECONDS
   // --------------------------------------
   if (m_timeTickMode == SuperSpectrogramConfig::TimeTickMode::Seconds)
   {
      const double totalDuration =
         static_cast<double>(m_data.GetNumSamples()) / (2 * m_data.GetMaxFreq());

      const double viewLeftTime =
         (viewLeftFrame / numFrames) * totalDuration;

      const double viewRightTime =
         (viewRightFrame / numFrames) * totalDuration;

      const double viewWidthSec = viewRightTime - viewLeftTime;

      // Tick selection
      static const double tickSteps[] = {
         0.005, 0.01, 0.05, 0.1, 0.5, 1, 2, 5,
         10, 20, 30, 60, 120, 300, 600, 1800
      };

      const double targetPixelsPerTick = 80.0;
      const double secondsPerPixel = viewWidthSec / size.GetWidth();

      double step = tickSteps[0];
      for (double t : tickSteps)
         if (t / secondsPerPixel >= targetPixelsPerTick) {
            step = t;
            break;
         }

      double tick = std::ceil(viewLeftTime / step) * step;

      while (tick <= viewRightTime)
      {
         double fx = (tick - viewLeftTime) / viewWidthSec * size.GetWidth();

         wxString label;
         if (tick < 60)
            label = wxString::Format("%g", tick);
         else
            label.Printf("%.0f:%02.0f",
               std::floor(tick / 60.0),
               std::fmod(tick, 60.0));

         DrawTickLabel(dc, size, fx, label);

         tick += step;
      }
   }

   // --------------------------------------
   // Mode: SAMPLES
   // --------------------------------------
   else if (m_timeTickMode == SuperSpectrogramConfig::TimeTickMode::Samples)
   {
      // Map frame -> sample
      const double samplesPerFrame =
         static_cast<double>(m_data.GetNumSamples()) / numFrames;

      const double viewLeftSample = viewLeftFrame * samplesPerFrame;
      const double viewRightSample = viewRightFrame * samplesPerFrame;

      const double viewWidthSamples = viewRightSample - viewLeftSample;

      // Tick steps in samples
      static const double tickSteps[] = {
         1, 10, 50, 100, 500, 1000, 5000,
         10000, 50000, 100000, 500000, 1000000
      };

      const double targetPixelsPerTick = 80.0;
      const double samplesPerPixel = viewWidthSamples / size.GetWidth();

      double step = tickSteps[0];
      for (double t : tickSteps)
         if (t / samplesPerPixel >= targetPixelsPerTick) {
            step = t;
            break;
         }

      double tick = std::ceil(viewLeftSample / step) * step;

      while (tick <= viewRightSample)
      {
         double fx = (tick - viewLeftSample) / viewWidthSamples * size.GetWidth();

         wxString label = wxString::Format("%.0f", tick);

         DrawTickLabel(dc, size, fx, label);

         tick += step;
      }
   }
}

void SuperSpectrogramPanel::DrawTickLabel(
   wxDC& dc,
   const wxSize& size,
   double fx,
   const wxString& label) const
{
   wxCoord tw, th;
   dc.GetTextExtent(label, &tw, &th);

   int x = static_cast<int>(fx - tw / 2);
   int y = size.GetHeight() - th - 2;

   dc.SetBrush(*wxBLACK_BRUSH);
   dc.SetPen(*wxTRANSPARENT_PEN);
   dc.DrawRectangle(x - 2, y - 1, tw + 4, th + 2);

   dc.SetPen(*wxWHITE_PEN);
   dc.DrawText(label, x, y);
}


void SuperSpectrogramPanel::SetShowNoteLines(bool show)
{
   if (m_showNoteLines == show)
      return;

   m_showNoteLines = show;
   Refresh();
}

void SuperSpectrogramPanel::SetTimeTickMode(SuperSpectrogramConfig::TimeTickMode mode)
{
   if (m_timeTickMode == mode)
      return;

   m_timeTickMode = mode;
   Refresh();  // trigger repaint
}
