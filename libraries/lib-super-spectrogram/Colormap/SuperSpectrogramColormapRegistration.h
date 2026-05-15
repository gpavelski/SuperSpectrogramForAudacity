/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapRegistration.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_COLORMAP_REGISTRATION__
#define __SUPER_SPECTROGRAM_COLORMAP_REGISTRATION__

#include "Colormap/SuperSpectrogramColormapRegistry.h"

class ColormapRegistrar
{
public:
   ColormapRegistrar(
      const wxString& id,
      const wxString& displayName,
      SuperSpectrogramColormapRegistry::Factory factory)
   {
      SuperSpectrogramColormapRegistry::Instance().Register(
         id,
         displayName,
         std::move(factory)
      );
   }
};

#endif