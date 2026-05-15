/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramPanel.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_PANEL__
#define __SUPER_SPECTROGRAM_PANEL__

#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/wx.h>

#include "Config/SuperSpectrogramConfig.h"
#include "Config/SuperSpectrogramConstants.h"
#include "DataAdapter/SuperSpectrogramDataAdapter.h"
#include "View/SuperSpectrogramMouseEvent.h"
#include "View/SuperSpectrogramRenderer.h"
#include "View/SuperSpectrogramViewModel.h"

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
   void SetColormap(const wxString& colormapId);

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
