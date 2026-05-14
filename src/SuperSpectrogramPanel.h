/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramPanel.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_PANEL__
#define __SUPER_SPECTROGRAM_PANEL__

#include <wx/wx.h>
#include <wx/dcbuffer.h>

#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramConstants.h"
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramMouseEvent.h"
#include "SuperSpectrogramRenderer.h"
#include "SuperSpectrogramViewModel.h"

class SuperSpectrogramPanel : public wxPanel
{
public:
   explicit SuperSpectrogramPanel(wxWindow* parent);

   // Data & configuration
   void SetData(const SuperSpectrogramFrame& frame);
   void ResetView();
   void Clear();

   size_t GetColumnCount() const;

   // Rendering
   wxBitmap RenderCurrentViewToBitmap() const;
   void Render(wxDC& dc, const wxSize& target) const;

   // View configuration
   void SetColormap(SuperSpectrogramConfig::Colormap type);

   void SetNoteNamingStyle(SuperSpectrogramConfig::NoteNaming style);

   void SetShowNoteLines(bool show);

   void SetTimeTickMode(SuperSpectrogramConfig::TimeTickMode mode);

   SuperSpectrogramConfig::TimeTickMode GetTimeTickMode() const;

private:
   // wxWidgets event handlers
   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMouse(wxMouseEvent& event);
   void OnRightClick(wxMouseEvent& event);
   void OnWheel(wxMouseEvent& event);

private:
   SuperSpectrogramViewModel m_vm;
   SuperSpectrogramRenderer  m_renderer;

   wxDECLARE_EVENT_TABLE();
};

#endif
