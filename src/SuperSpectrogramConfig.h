/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramConfig.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_CONFIG__
#define __SUPER_SPECTROGRAM_CONFIG__

#include "SuperSpectrogramConstants.h"

class SuperSpectrogramConfig
{
public:
   // =========================
   // Domain enums (single source of truth)
   // =========================

   enum class Colormap
   {
      Jet,
      Gray,
      Hot,
      Viridis,
      Inferno,
      Magma,
      Cividis,
      Parula
   };

   enum class NoteNaming
   {
      Sharps,
      Flats,
      Mixed
   };

   enum class TimeTickMode
   {
      None,
      Seconds,
      Samples
   };

   // =========================
   // State
   // =========================

   int noiseFloor = -70;
   int detailLevel = 7;

   Colormap colormap = Colormap::Jet;
   NoteNaming noteNaming = NoteNaming::Mixed;
   bool showNoteLines = true;
   TimeTickMode timeTickMode = TimeTickMode::Seconds;

   struct WindowGeometry
   {
      int width = SuperSpectrogramConstants::UI::kDefaultWidth;
      int height = SuperSpectrogramConstants::UI::kDefaultHeight;
      int posX = wxDefaultCoord;
      int posY = wxDefaultCoord;
      bool valid = false;
   };

   WindowGeometry window;

   // =========================
   // Persistence
   // =========================

   void Load();
   void Save() const;

private:
   static const wxString kConfigPath;
};

#endif
