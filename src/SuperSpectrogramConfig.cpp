/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramConfig.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramConfig.h"
#include <wx/config.h>

const wxString SuperSpectrogramConfig::kConfigPath = "/SuperSpectrogram";

void SuperSpectrogramConfig::Load()
{
   wxConfigBase* cfg = wxConfigBase::Get(false);
   if (!cfg)
      return;

   cfg->SetPath(kConfigPath);

   long value;

   if (cfg->Read("NoiseFloor", &value))
      noiseFloor = static_cast<int>(value);

   if (cfg->Read("HighestNote", &value))
      detailLevel = static_cast<int>(value);

   if (cfg->Read("Colormap", &value))
      colormap = static_cast<SuperSpectrogramConfig::Colormap>(value);

   if (cfg->Read("NoteNaming", &value))
      noteNaming = static_cast<SuperSpectrogramConfig::NoteNaming>(value);

   cfg->Read("ShowNoteLines", &showNoteLines);

   if (cfg->Read("TimeTickMode", &value))
      timeTickMode = static_cast<SuperSpectrogramConfig::TimeTickMode>(value);
}

void SuperSpectrogramConfig::Save() const
{
   wxConfigBase* cfg = wxConfigBase::Get(false);
   if (!cfg)
      return;

   cfg->SetPath(kConfigPath);

   cfg->Write("NoiseFloor", (long)noiseFloor);
   cfg->Write("HighestNote", (long)detailLevel);
   cfg->Write("Colormap", (long)colormap);
   cfg->Write("NoteNaming", (long)noteNaming);
   cfg->Write("ShowNoteLines", showNoteLines);
   cfg->Write("TimeTickMode", (long)timeTickMode);

   cfg->Flush();
}
