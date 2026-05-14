/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapFactory.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramColormapFactory.h"
#include "SuperSpectrogramColormapUtils.h"

namespace
{
   using ColormapType = SuperSpectrogramConfig::Colormap;

   std::unique_ptr<IColormap> MakeColormap(ColormapType type)
   {
      using namespace ColormapUtils;

      switch (type)
      {
      case ColormapType::Jet:      return std::make_unique<LUTColormap>(MakeJet());
      case ColormapType::Gray:     return std::make_unique<LUTColormap>(MakeGray());
      case ColormapType::Hot:      return std::make_unique<LUTColormap>(MakeHot());
      case ColormapType::Viridis:  return std::make_unique<LUTColormap>(MakeViridis());
      case ColormapType::Inferno:  return std::make_unique<LUTColormap>(MakeInferno());
      case ColormapType::Magma:    return std::make_unique<LUTColormap>(MakeMagma());
      case ColormapType::Cividis:  return std::make_unique<LUTColormap>(MakeCividis());
      case ColormapType::Parula:   return std::make_unique<LUTColormap>(MakeParula());
      }

      return std::make_unique<LUTColormap>(MakeJet());
   }
}

std::unique_ptr<IColormap>
SuperSpectrogramColormapFactory::Create(SuperSpectrogramConfig::Colormap type)
{
   return MakeColormap(type);
}
