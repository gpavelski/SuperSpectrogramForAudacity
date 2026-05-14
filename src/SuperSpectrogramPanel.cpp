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
   : wxPanel(parent)
{
   SetBackgroundStyle(wxBG_STYLE_PAINT);
   SetDoubleBuffered(true);

   m_colormap = SuperSpectrogramColormapFactory::Create(
      SuperSpectrogramConfig::Colormap::Jet);

   auto notes = std::make_unique<SuperSpectrogramNotesLinesOverlay>(
      SuperSpectrogramConfig::NoteNaming::Mixed
   );
   m_notesOverlay = notes.get();
   m_overlays.push_back(std::move(notes));

   auto time = std::make_unique<SuperSpectrogramTimeTicksOverlay>(
      SuperSpectrogramConfig::TimeTickMode::Seconds
   );

   m_timeOverlay = time.get();
   m_overlays.push_back(std::move(time));
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

void SuperSpectrogramPanel::SetNoteNamingStyle(
   SuperSpectrogramConfig::NoteNaming style)
{
   if (m_notesOverlay)
      m_notesOverlay->SetNoteNamingStyle(style);

   Refresh();
}

void SuperSpectrogramPanel::SetShowNoteLines(bool show)
{
   if (m_notesOverlay)
      m_notesOverlay->SetEnabled(show);

   Refresh();
}

void SuperSpectrogramPanel::SetTimeTickMode(
   SuperSpectrogramConfig::TimeTickMode mode)
{
   if (m_timeOverlay)
      m_timeOverlay->SetMode(mode);

   Refresh();
}

SuperSpectrogramConfig::TimeTickMode
SuperSpectrogramPanel::GetTimeTickMode() const
{
   if (m_timeOverlay)
      return m_timeOverlay->GetMode();

   return SuperSpectrogramConfig::TimeTickMode::None;
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

   for (const auto& overlay : m_overlays)
   {
      if (overlay->IsEnabled())
         overlay->Render(dc, target, m_data, m_viewport);
   }
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
   if (m_interactionController.OnWheel(
      event,
      m_viewport,
      GetClientSize()
   ))
   {
      Refresh();
   }
}

//----------------------------------------------------------------------
// Handles mouse press, release, and drag events to support panning
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnMouse(wxMouseEvent& event)
{
   m_interactionController.OnMouse(
      event,
      m_viewport,
      GetClientSize(),
      *this
   );

   Refresh();
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
