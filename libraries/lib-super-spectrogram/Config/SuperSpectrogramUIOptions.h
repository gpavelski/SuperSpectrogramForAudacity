/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramUIOptions.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <vector>

#include "Config/SuperSpectrogramConfig.h"
#include "Colormap/SuperSpectrogramColormapRegistry.h"

template<typename T>
struct SuperSpectrogramOption
{
   wxString label;
   T value;
};

class SuperSpectrogramUIOptions
{
public:

   // =====================
   // Option lists
   // =====================

   template<typename T>
   static const std::vector<SuperSpectrogramOption<T>>& NoiseFloorOptions();

   template<typename T>
   static const std::vector<SuperSpectrogramOption<T>>& DetailLevelOptions();

   template<typename T>
   static const std::vector<SuperSpectrogramOption<T>>& ColormapOptions();

   template<typename T>
   static const std::vector<SuperSpectrogramOption<T>>& NoteNamingOptions();

   template<typename T>
   static const std::vector<SuperSpectrogramOption<T>>& TimeTickOptions();

   // =====================
   // Default values
   // =====================

   template<typename T>
   static T DefaultNoiseFloor();

   template<typename T>
   static T DefaultDetailLevel();

   template<typename T>
   static T DefaultColormap();

   template<typename T>
   static T DefaultNoteNaming();

   template<typename T>
   static T DefaultTimeTick();
};

