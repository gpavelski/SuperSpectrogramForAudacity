/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramPanel.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramPanel.h"

#include <wx/dcclient.h>
#include <wx/dcmemory.h>

//------------------------------------------------------------
// Event table
//------------------------------------------------------------
wxBEGIN_EVENT_TABLE(SuperSpectrogramPanel, wxPanel)
EVT_PAINT(SuperSpectrogramPanel::OnPaint)
EVT_SIZE(SuperSpectrogramPanel::OnSize)
EVT_LEFT_DOWN(SuperSpectrogramPanel::OnMouse)
EVT_LEFT_UP(SuperSpectrogramPanel::OnMouse)
EVT_MOTION(SuperSpectrogramPanel::OnMouse)
EVT_RIGHT_DOWN(SuperSpectrogramPanel::OnRightClick)
EVT_MOUSEWHEEL(SuperSpectrogramPanel::OnWheel)
wxEND_EVENT_TABLE()

//------------------------------------------------------------
// Construction
//------------------------------------------------------------
SuperSpectrogramPanel::SuperSpectrogramPanel(wxWindow* parent)
   : wxPanel(parent)
{
   SetBackgroundStyle(wxBG_STYLE_PAINT);
   SetDoubleBuffered(true);
}

//------------------------------------------------------------
// Data & Configuration API
//------------------------------------------------------------
void SuperSpectrogramPanel::SetData(const SuperSpectrogramFrame& frame)
{
   m_vm.SetData(frame);
   Refresh();
}

void SuperSpectrogramPanel::SetColormap(
   SuperSpectrogramConfig::Colormap type)
{
   m_vm.SetColormap(type);
   Refresh();
}

void SuperSpectrogramPanel::SetNoteNamingStyle(
   SuperSpectrogramConfig::NoteNaming style)
{
   m_vm.SetNoteNamingStyle(style);
   Refresh();
}

void SuperSpectrogramPanel::SetShowNoteLines(bool show)
{
   m_vm.SetShowNoteLines(show);
   Refresh();
}

void SuperSpectrogramPanel::SetTimeTickMode(
   SuperSpectrogramConfig::TimeTickMode mode)
{
   m_vm.SetTimeTickMode(mode);
   Refresh();
}

SuperSpectrogramConfig::TimeTickMode
SuperSpectrogramPanel::GetTimeTickMode() const
{
   return m_vm.GetTimeTickMode();
}

size_t SuperSpectrogramPanel::GetColumnCount() const
{
   return m_vm.GetColumnCount();
}

//----------------------------------------------------------------------
// Rendering
//----------------------------------------------------------------------
void SuperSpectrogramPanel::Render(wxDC& dc, const wxSize& target) const
{
   dc.SetBackground(*wxBLACK_BRUSH);
   dc.Clear();

   if (!m_vm.HasData())
      return;

   auto ctx = m_vm.BuildRenderContext();
   wxBitmap bmp = m_renderer.Render(ctx, target);

   dc.DrawBitmap(bmp, 0, 0);

   m_vm.RenderOverlays(dc, target);
}

//----------------------------------------------------------------------
// Bitmap Export
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
// Interaction Handling
//----------------------------------------------------------------------
void SuperSpectrogramPanel::OnWheel(wxMouseEvent& event)
{
   SuperSpectrogramMouseEvent e;
   e.x = event.GetX();
   e.y = event.GetY();
   e.wheelRotation = event.GetWheelRotation();

   if (m_vm.HandleWheel(e, GetClientSize()))
      Refresh();
}

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

   if (m_vm.HandleMouse(e, GetClientSize()))
      Refresh();
}

//----------------------------------------------------------------------
// View Control
//----------------------------------------------------------------------
void SuperSpectrogramPanel::ResetView()
{
   m_vm.ResetView();
   Refresh(false);
}

void SuperSpectrogramPanel::OnRightClick(wxMouseEvent& event)
{
   ResetView();
   event.Skip();
}

void SuperSpectrogramPanel::OnPaint(wxPaintEvent&)
{
   wxAutoBufferedPaintDC dc(this);
   Render(dc, GetClientSize());
}

void SuperSpectrogramPanel::OnSize(wxSizeEvent& event)
{
   if (m_vm.HasData()) {
      Refresh();
   }
   event.Skip();
}
