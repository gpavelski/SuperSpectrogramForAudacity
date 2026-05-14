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
   // Domain configuration enums
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
   // Runtime configuration state
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
   // Persistence API
   // =========================

   void Load();
   void Save() const;

private:
   static const wxString kConfigPath;
};

#endif
