/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramMagmaColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class MagmaColormap final : public LUTColormap
   {
   public:
      MagmaColormap()
         :
         LUTColormap(
            "magma",
            "Magma",
            ColormapUtils::MakeMagma())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "magma",
      "Magma",
      []()
      {
         return std::make_unique<MagmaColormap>();
      });
}