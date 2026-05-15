/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramUIOptions.cpp

  Guilherme Pavelski

**********************************************************************/

#include "Config/SuperSpectrogramUIOptions.h"

namespace
{
   // ---------------------------------------------------------
   // Noise floor
   // ---------------------------------------------------------
   static const std::vector<SuperSpectrogramOption<int>> kNoiseFloorOptions{
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
   static const std::vector<SuperSpectrogramOption<int>> kDetailLevelOptions{
       { "C4", 4 },
       { "C5", 5 },
       { "C6", 6 },
       { "C7", 7 },
       { "C8", 8 }
   };
   constexpr int kDefaultDetailLevel = 7;

   // ---------------------------------------------------------
   // Note naming
   // ---------------------------------------------------------
   static const std::vector<SuperSpectrogramOption<int>> kNoteNamingOptions{
       { "Sharps (C#)", static_cast<int>(SuperSpectrogramConfig::NoteNaming::Sharps) },
       { "Flats (Db)",  static_cast<int>(SuperSpectrogramConfig::NoteNaming::Flats) },
       { "Mixed",       static_cast<int>(SuperSpectrogramConfig::NoteNaming::Mixed) }
   };
   constexpr int kDefaultNoteNaming = static_cast<int>(SuperSpectrogramConfig::NoteNaming::Mixed);

   // ---------------------------------------------------------
   // Time tick mode
   // ---------------------------------------------------------
   static const std::vector<SuperSpectrogramOption<int>> kTimeTickOptions{
       { "Seconds", static_cast<int>(SuperSpectrogramConfig::TimeTickMode::Seconds) },
       { "Samples", static_cast<int>(SuperSpectrogramConfig::TimeTickMode::Samples) },
       { "None",    static_cast<int>(SuperSpectrogramConfig::TimeTickMode::None) }
   };
   constexpr int kDefaultTimeTick = static_cast<int>(SuperSpectrogramConfig::TimeTickMode::Seconds);

   // ---------------------------------------------------------
   // Default colormap
   // ---------------------------------------------------------
   static const wxString kDefaultColormap = "jet";
}

// ------------------------------------------------------------
// Noise floor
// ------------------------------------------------------------
template<>
const std::vector<SuperSpectrogramOption<int>>&
SuperSpectrogramUIOptions::NoiseFloorOptions<int>()
{
   return kNoiseFloorOptions;
}

template<>
int SuperSpectrogramUIOptions::DefaultNoiseFloor<int>()
{
   return kDefaultNoiseFloor;
}

// ------------------------------------------------------------
// Detail level
// ------------------------------------------------------------
template<>
const std::vector<SuperSpectrogramOption<int>>&
SuperSpectrogramUIOptions::DetailLevelOptions<int>()
{
   return kDetailLevelOptions;
}

template<>
int SuperSpectrogramUIOptions::DefaultDetailLevel<int>()
{
   return kDefaultDetailLevel;
}

// ------------------------------------------------------------
// Colormap (runtime from registry)
// ------------------------------------------------------------
template<>
const std::vector<SuperSpectrogramOption<wxString>>&
SuperSpectrogramUIOptions::ColormapOptions<wxString>()
{
   static std::vector<SuperSpectrogramOption<wxString>> options;

   options.clear(); // rebuild in case registry changed
   const auto& entries = SuperSpectrogramColormapRegistry::Instance().GetEntries();

   for (const auto& e : entries)
   {
      options.push_back({ e.displayName, e.id });
   }

   return options;
}

template<>
wxString SuperSpectrogramUIOptions::DefaultColormap<wxString>()
{
   return kDefaultColormap;
}

// ------------------------------------------------------------
// Note naming
// ------------------------------------------------------------
template<>
const std::vector<SuperSpectrogramOption<int>>&
SuperSpectrogramUIOptions::NoteNamingOptions<int>()
{
   return kNoteNamingOptions;
}

template<>
int SuperSpectrogramUIOptions::DefaultNoteNaming<int>()
{
   return kDefaultNoteNaming;
}

// ------------------------------------------------------------
// Time tick mode
// ------------------------------------------------------------
template<>
const std::vector<SuperSpectrogramOption<int>>&
SuperSpectrogramUIOptions::TimeTickOptions<int>()
{
   return kTimeTickOptions;
}

template<>
int SuperSpectrogramUIOptions::DefaultTimeTick<int>()
{
   return kDefaultTimeTick;
}
