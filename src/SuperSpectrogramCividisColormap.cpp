/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramCividisColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapUtils.h"
#include "SuperSpectrogramColormapRegistration.h"

namespace
{
   class CividisColormap final : public LUTColormap
   {
   public:
      CividisColormap()
         :
         LUTColormap(
            "cividis",
            "Cividis",
            ColormapUtils::MakeCividis())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "cividis",
      "Cividis",
      []()
      {
         return std::make_unique<CividisColormap>();
      });
}