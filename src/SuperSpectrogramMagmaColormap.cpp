/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramMagmaColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapUtils.h"
#include "SuperSpectrogramColormapRegistration.h"

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