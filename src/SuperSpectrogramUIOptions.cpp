/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramUIOptions.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramUIOptions.h"
#include "SuperSpectrogramConfig.h"

namespace
{
   // ---------------------------------------------------------
   // Noise floor
   // ---------------------------------------------------------

   const std::vector<SuperSpectrogramOption> kNoiseFloorOptions{
      { "-120 dB", -120 },
      { "-100 dB", -100 },
      {  "-85 dB",  -85 },
      {  "-70 dB",  -70 },
      {  "-55 dB",  -55 }
   };

   constexpr int kDefaultNoiseFloor = -70;

   // ---------------------------------------------------------
   // Detail level / highest note
   // ---------------------------------------------------------

   const std::vector<SuperSpectrogramOption> kDetailLevelOptions{
      { "C4", 4 },
      { "C5", 5 },
      { "C6", 6 },
      { "C7", 7 },
      { "C8", 8 }
   };

   constexpr int kDefaultDetailLevel = 7;

   // ---------------------------------------------------------
   // Colormap
   // ---------------------------------------------------------

   const std::vector<SuperSpectrogramOption> kColormapOptions{
      {
         "Jet",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Jet)
      },
      {
         "Gray",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Gray)
      },
      {
         "Hot",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Hot)
      },
      {
         "Viridis",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Viridis)
      },
      {
         "Inferno",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Inferno)
      },
      {
         "Magma",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Magma)
      },
      {
         "Cividis",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Cividis)
      },
      {
         "Parula",
         static_cast<int>(SuperSpectrogramConfig::Colormap::Parula)
      }
   };

   constexpr int kDefaultColormap =
      static_cast<int>(SuperSpectrogramConfig::Colormap::Jet);

   // ---------------------------------------------------------
   // Note naming
   // ---------------------------------------------------------

   const std::vector<SuperSpectrogramOption> kNoteNamingOptions{
      {
         "Sharps (C#)",
         static_cast<int>(
            SuperSpectrogramConfig::NoteNaming::Sharps)
      },
      {
         "Flats (Db)",
         static_cast<int>(
            SuperSpectrogramConfig::NoteNaming::Flats)
      },
      {
         "Mixed",
         static_cast<int>(
            SuperSpectrogramConfig::NoteNaming::Mixed)
      }
   };

   constexpr int kDefaultNoteNaming =
      static_cast<int>(
         SuperSpectrogramConfig::NoteNaming::Mixed);

   // ---------------------------------------------------------
   // Time tick mode
   // ---------------------------------------------------------

   const std::vector<SuperSpectrogramOption> kTimeTickOptions{
      {
         "Seconds",
         static_cast<int>(
            SuperSpectrogramConfig::TimeTickMode::Seconds)
      },
      {
         "Samples",
         static_cast<int>(
            SuperSpectrogramConfig::TimeTickMode::Samples)
      },
      {
         "None",
         static_cast<int>(
            SuperSpectrogramConfig::TimeTickMode::None)
      }
   };

   constexpr int kDefaultTimeTick =
      static_cast<int>(
         SuperSpectrogramConfig::TimeTickMode::Seconds);
}

// ------------------------------------------------------------
// Noise floor
// ------------------------------------------------------------

const std::vector<SuperSpectrogramOption>&
SuperSpectrogramUIOptions::NoiseFloorOptions()
{
   return kNoiseFloorOptions;
}

int SuperSpectrogramUIOptions::DefaultNoiseFloor()
{
   return kDefaultNoiseFloor;
}

// ------------------------------------------------------------
// Detail level
// ------------------------------------------------------------

const std::vector<SuperSpectrogramOption>&
SuperSpectrogramUIOptions::DetailLevelOptions()
{
   return kDetailLevelOptions;
}

int SuperSpectrogramUIOptions::DefaultDetailLevel()
{
   return kDefaultDetailLevel;
}

// ------------------------------------------------------------
// Colormap
// ------------------------------------------------------------

const std::vector<SuperSpectrogramOption>&
SuperSpectrogramUIOptions::ColormapOptions()
{
   return kColormapOptions;
}

int SuperSpectrogramUIOptions::DefaultColormap()
{
   return kDefaultColormap;
}

// ------------------------------------------------------------
// Note naming
// ------------------------------------------------------------

const std::vector<SuperSpectrogramOption>&
SuperSpectrogramUIOptions::NoteNamingOptions()
{
   return kNoteNamingOptions;
}

int SuperSpectrogramUIOptions::DefaultNoteNaming()
{
   return kDefaultNoteNaming;
}

// ------------------------------------------------------------
// Time tick mode
// ------------------------------------------------------------

const std::vector<SuperSpectrogramOption>&
SuperSpectrogramUIOptions::TimeTickOptions()
{
   return kTimeTickOptions;
}

int SuperSpectrogramUIOptions::DefaultTimeTick()
{
   return kDefaultTimeTick;
}