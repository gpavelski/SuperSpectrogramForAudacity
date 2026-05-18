/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapRegistration.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

#include "Colormap/SuperSpectrogramColormapRegistry.h"

template<const wxChar* ID, const wxChar* NAME, auto ColorFunc>
class ColormapRegistrar
{
public:
   ColormapRegistrar()
   {
      SuperSpectrogramColormapRegistry::Instance().Register(
         ID,
         NAME,
         []() -> std::unique_ptr<IColormap>
         {
            return std::make_unique<LUTColormapGenerator<ID, NAME, ColorFunc>>();
         }
      );
   }
};
