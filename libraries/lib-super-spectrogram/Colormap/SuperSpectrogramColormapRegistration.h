/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapRegistration.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

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
