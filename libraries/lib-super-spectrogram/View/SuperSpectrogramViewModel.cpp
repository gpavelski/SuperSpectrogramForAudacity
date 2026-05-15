/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramViewModel.cpp

  Guilherme Pavelski

**********************************************************************/

#include "View/SuperSpectrogramViewModel.h"

/**
 * Initializes default rendering state and registers built-in overlays.
 */
SuperSpectrogramViewModel::SuperSpectrogramViewModel()
{
   SetColormap("jet");

   auto notes = std::make_unique<SuperSpectrogramNotesLinesOverlay>(
      SuperSpectrogramConfig::NoteNaming::Mixed);

   m_notesOverlay = notes.get();
   m_overlays.push_back(std::move(notes));

   auto time = std::make_unique<SuperSpectrogramTimeTicksOverlay>(
      SuperSpectrogramConfig::TimeTickMode::Seconds);

   m_timeOverlay = time.get();
   m_overlays.push_back(std::move(time));
}

// =========================================================
// Data lifecycle
// =========================================================

void SuperSpectrogramViewModel::SetData(const SuperSpectrogramFrame& frame)
{
   m_data.SetFrame(frame);

   m_viewport.SetBounds(
      m_data.Rows(),
      m_data.Cols()
   );
}

void SuperSpectrogramViewModel::ResetView()
{
   m_viewport.Reset();
}

void SuperSpectrogramViewModel::Clear()
{
   m_data.Clear();
   m_viewport.Reset();
}

size_t SuperSpectrogramViewModel::GetColumnCount() const
{
   return m_data.GetNormalized().empty()
      ? 0
      : m_data.Cols();
}

// =========================================================
// Configuration
// =========================================================

void SuperSpectrogramViewModel::SetColormap(
   const wxString& id)
{
   auto cmap =
      SuperSpectrogramColormapRegistry
      ::Instance()
      .Create(id);

   if (cmap)
   {
      m_colormap = std::move(cmap);
   }
   else
   {
      // Fallback for invalid config/plugin removal
      m_colormap =
         SuperSpectrogramColormapRegistry
         ::Instance()
         .Create("jet");
   }
}

void SuperSpectrogramViewModel::SetNoteNamingStyle(
   SuperSpectrogramConfig::NoteNaming style)
{
   if (m_notesOverlay)
      m_notesOverlay->SetNoteNamingStyle(style);
}

void SuperSpectrogramViewModel::SetShowNoteLines(bool show)
{
   if (m_notesOverlay)
      m_notesOverlay->SetEnabled(show);
}

void SuperSpectrogramViewModel::SetTimeTickMode(
   SuperSpectrogramConfig::TimeTickMode mode)
{
   if (m_timeOverlay)
      m_timeOverlay->SetMode(mode);
}

SuperSpectrogramConfig::TimeTickMode
SuperSpectrogramViewModel::GetTimeTickMode() const
{
   return m_timeOverlay
      ? m_timeOverlay->GetMode()
      : SuperSpectrogramConfig::TimeTickMode::None;
}

// =========================================================
// Interaction
// =========================================================

bool SuperSpectrogramViewModel::HandleMouse(
   const SuperSpectrogramMouseEvent& e,
   const wxSize& size)
{
   return m_interactionController.OnMouse(e, m_viewport, size);
}

bool SuperSpectrogramViewModel::HandleWheel(
   const SuperSpectrogramMouseEvent& e,
   const wxSize& size)
{
   return m_interactionController.OnWheel(e, m_viewport, size);
}

// =========================================================
// Rendering
// =========================================================

bool SuperSpectrogramViewModel::HasData() const
{
   return !m_data.GetNormalized().empty() && m_colormap != nullptr;
}

SuperSpectrogramRenderContext
SuperSpectrogramViewModel::BuildRenderContext() const
{
   return SuperSpectrogramRenderContext{
      m_data.GetNormalized(),
      m_data.Rows(),
      m_data.Cols(),
      *m_colormap,
      m_viewport
   };
}

void SuperSpectrogramViewModel::RenderOverlays(
   wxDC& dc,
   const wxSize& size) const
{
   for (const auto& overlay : m_overlays)
   {
      if (!overlay->IsEnabled())
         continue;

      overlay->Render(
         dc,
         size,
         m_data,
         m_viewport
      );
   }
}
