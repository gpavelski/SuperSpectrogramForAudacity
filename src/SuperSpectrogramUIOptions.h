/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramUIOptions.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_UI_OPTIONS__
#define __SUPER_SPECTROGRAM_UI_OPTIONS__

struct SuperSpectrogramOption
{
   wxString label;
   int value;
};

class SuperSpectrogramUIOptions
{
public:
   static const std::vector<SuperSpectrogramOption>& NoiseFloorOptions();
   static const std::vector<SuperSpectrogramOption>& DetailLevelOptions();
   static const std::vector<SuperSpectrogramOption>& ColormapOptions();
   static const std::vector<SuperSpectrogramOption>& NoteNamingOptions();
   static const std::vector<SuperSpectrogramOption>& TimeTickOptions();

   static int DefaultNoiseFloor();
   static int DefaultDetailLevel();
   static int DefaultColormap();
   static int DefaultNoteNaming();
   static int DefaultTimeTick();
};

#endif