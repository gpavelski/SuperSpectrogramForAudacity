/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramSettings.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramSettings.h"
#include <wx/config.h>

const wxString SuperSpectrogramSettings::kConfigPath = "/SuperSpectrogram";

void SuperSpectrogramSettings::Load()
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
      colormap = static_cast<int>(value);

   if (cfg->Read("NoteNaming", &value))
      noteNaming = static_cast<int>(value);

   cfg->Read("ShowNoteLines", &showNoteLines);

   if (cfg->Read("TimeTickMode", &value))
      timeTickMode = static_cast<int>(value);
}

void SuperSpectrogramSettings::Save() const
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
