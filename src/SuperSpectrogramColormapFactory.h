/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapFactory.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_COLORMAP_FACTORY__
#define __SUPER_SPECTROGRAM_COLORMAP_FACTORY__

#include <memory>
#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramColormap.h"

class SuperSpectrogramColormapFactory
{
public:
   static std::unique_ptr<IColormap> Create(
      SuperSpectrogramConfig::Colormap type);
};

#endif
