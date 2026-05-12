/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapFactory.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramColormapFactory.h"
#include "SuperSpectrogramColormapUtils.h"

std::unique_ptr<IColormap>
SuperSpectrogramColormapFactory::Create(
   SuperSpectrogramConfig::Colormap type)
{
   using namespace ColormapUtils;

   switch (type)
   {
   case SuperSpectrogramConfig::Colormap::Jet:
      return std::make_unique<LUTColormap>(MakeJet());

   case SuperSpectrogramConfig::Colormap::Gray:
      return std::make_unique<LUTColormap>(MakeGray());

   case SuperSpectrogramConfig::Colormap::Hot:
      return std::make_unique<LUTColormap>(MakeHot());

   case SuperSpectrogramConfig::Colormap::Viridis:
      return std::make_unique<LUTColormap>(MakeViridis());

   case SuperSpectrogramConfig::Colormap::Inferno:
      return std::make_unique<LUTColormap>(MakeInferno());

   case SuperSpectrogramConfig::Colormap::Magma:
      return std::make_unique<LUTColormap>(MakeMagma());

   case SuperSpectrogramConfig::Colormap::Cividis:
      return std::make_unique<LUTColormap>(MakeCividis());

   case SuperSpectrogramConfig::Colormap::Parula:
      return std::make_unique<LUTColormap>(MakeParula());
   }

   return std::make_unique<LUTColormap>(MakeJet());
}
