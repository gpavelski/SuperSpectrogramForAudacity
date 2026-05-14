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

   SuperSpectrogramRenderContext ctx{
      m_data.GetNormalized(),
      m_data.Rows(),
      m_data.Cols(),
      *m_colormap,
      m_viewport
   };

   wxBitmap bmp = m_renderer.Render(ctx, target);

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
   SuperSpectrogramMouseEvent e;
   e.x = event.GetX();
   e.y = event.GetY();
   e.wheelRotation = event.GetWheelRotation();

   if (m_interactionController.OnWheel(e, m_viewport, GetClientSize()))
      Refresh();
}

//----------------------------------------------------------------------
// Handles mouse press, release, and drag events to support panning
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnMouse(wxMouseEvent& event)
{
   SuperSpectrogramMouseEvent e;
   e.x = event.GetX();
   e.y = event.GetY();
   e.leftDown = event.LeftDown();
   e.leftUp = event.LeftUp();
   e.dragging = event.Dragging() && event.LeftIsDown();

   if (event.LeftDown())
      CaptureMouse();
   else if (event.LeftUp() && HasCapture())
      ReleaseMouse();

   if (m_interactionController.OnMouse(e, m_viewport, GetClientSize()))
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
