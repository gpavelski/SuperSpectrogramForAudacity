#ifndef __SUPER_SPECTROGRAM_SETTINGS__
#define __SUPER_SPECTROGRAM_SETTINGS__

#include <wx/string.h>

class SuperSpectrogramSettings
{
public:
   void Load();
   void Save() const;

   // Persisted parameters
   int noiseFloor = -70;
   int detailLevel = 7;
   int colormap = 0;
   int noteNaming = 0;
   bool showNoteLines = true;
   int timeTickMode = 0;

private:
   static const wxString kConfigPath;
};

#endif