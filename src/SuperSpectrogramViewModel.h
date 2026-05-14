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
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramFrame.h"
#include "SuperSpectrogramInteractionController.h"
#include "SuperSpectrogramMouseEvent.h" 
#include "SuperSpectrogramViewport.h"
#include "SuperSpectrogramOverlay.h"

// Forward declarations
class SuperSpectrogramNotesLinesOverlay;
class SuperSpectrogramTimeTicksOverlay;

struct SuperSpectrogramRenderContext;

class SuperSpectrogramViewModel
{
public:
   SuperSpectrogramViewModel();

   // --------------------------
   // Data
   // --------------------------
   void SetData(const SuperSpectrogramFrame& frame);
   void ResetView();
   void Clear();

   size_t GetColumnCount() const;

   // --------------------------
   // Configuration
   // --------------------------
   void SetColormap(SuperSpectrogramConfig::Colormap type);

   void SetNoteNamingStyle(SuperSpectrogramConfig::NoteNaming style);
   void SetShowNoteLines(bool show);

   void SetTimeTickMode(SuperSpectrogramConfig::TimeTickMode mode);
   SuperSpectrogramConfig::TimeTickMode GetTimeTickMode() const;

   bool HandleMouse(
      const SuperSpectrogramMouseEvent& e,
      const wxSize& size);

   // --------------------------
   // Interaction
   // --------------------------
   bool HandleWheel(
      const SuperSpectrogramMouseEvent& e,
      const wxSize& size);

   // --------------------------
   // Rendering
   // --------------------------
   bool HasData() const;

   SuperSpectrogramRenderContext BuildRenderContext() const;

   const std::vector<std::unique_ptr<ISpectrogramOverlay>>&
      GetOverlays() const { return m_overlays; }

   const SuperSpectrogramDataAdapter& GetData() const { return m_data; }

   void RenderOverlays(
      wxDC& dc,
      const wxSize& size) const;

private:
   std::unique_ptr<IColormap> m_colormap;
   SuperSpectrogramDataAdapter m_data;
   SuperSpectrogramViewport m_viewport;

   std::vector<std::unique_ptr<ISpectrogramOverlay>> m_overlays;
   SuperSpectrogramInteractionController m_interactionController;

   // Optional typed access
   SuperSpectrogramNotesLinesOverlay* m_notesOverlay = nullptr;
   SuperSpectrogramTimeTicksOverlay* m_timeOverlay = nullptr;
};

#endif
