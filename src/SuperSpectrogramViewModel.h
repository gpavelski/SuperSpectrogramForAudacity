/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramViewModel.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_VIEW_MODEL__
#define __SUPER_SPECTROGRAM_VIEW_MODEL__

#include <memory>
#include <vector>

#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapRegistry.h"
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramFrame.h"
#include "SuperSpectrogramInteractionController.h"
#include "SuperSpectrogramMouseEvent.h"
#include "SuperSpectrogramViewport.h"
#include "SuperSpectrogramOverlay.h"
#include "SuperSpectrogramRenderContext.h"

class SuperSpectrogramNotesLinesOverlay;
class SuperSpectrogramTimeTicksOverlay;

struct SuperSpectrogramRenderContext;

/**
 * ViewModel:
 * Owns all non-UI spectrogram state:
 * - data
 * - viewport
 * - overlays
 * - interaction logic
 * - render configuration (colormap)
 */
class SuperSpectrogramViewModel
{
public:
   SuperSpectrogramViewModel();

   // =========================================================
   // Data lifecycle
   // =========================================================
   void SetData(const SuperSpectrogramFrame& frame);
   void ResetView();
   void Clear();

   size_t GetColumnCount() const;

   // =========================================================
   // Configuration
   // =========================================================
   void SetColormap(
      const wxString& id);

   void SetNoteNamingStyle(SuperSpectrogramConfig::NoteNaming style);
   void SetShowNoteLines(bool show);

   void SetTimeTickMode(SuperSpectrogramConfig::TimeTickMode mode);
   SuperSpectrogramConfig::TimeTickMode GetTimeTickMode() const;

   // =========================================================
   // Interaction
   // =========================================================
   bool HandleMouse(const SuperSpectrogramMouseEvent& e, const wxSize& size);
   bool HandleWheel(const SuperSpectrogramMouseEvent& e, const wxSize& size);

   // =========================================================
   // Rendering
   // =========================================================
   bool HasData() const;

   SuperSpectrogramRenderContext BuildRenderContext() const;

   const std::vector<std::unique_ptr<ISpectrogramOverlay>>& GetOverlays() const
   {
      return m_overlays;
   }

   const SuperSpectrogramDataAdapter& GetData() const { return m_data; }

   void RenderOverlays(wxDC& dc, const wxSize& size) const;

private:
   // Core rendering state
   std::unique_ptr<IColormap> m_colormap;
   SuperSpectrogramDataAdapter m_data;
   SuperSpectrogramViewport m_viewport;

   // Interaction controller (pan/zoom logic)
   SuperSpectrogramInteractionController m_interactionController;

   // Owned overlays
   std::vector<std::unique_ptr<ISpectrogramOverlay>> m_overlays;

   // Optional direct overlay access (kept for targeted updates)
   SuperSpectrogramNotesLinesOverlay* m_notesOverlay = nullptr;
   SuperSpectrogramTimeTicksOverlay* m_timeOverlay = nullptr;
};

#endif
