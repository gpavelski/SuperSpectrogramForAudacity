/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramParulaColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapUtils.h"
#include "SuperSpectrogramColormapRegistration.h"

namespace
{
   class ParulaColormap final : public LUTColormap
   {
   public:
      ParulaColormap()
         :
         LUTColormap(
            "parula",
            "Parula",
            ColormapUtils::MakeParula())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "parula",
      "Parula",
      []()
      {
         return std::make_unique<ParulaColormap>();
      });
}