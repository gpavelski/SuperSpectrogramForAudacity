/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"

wxColour LUTColormap::Map(double v) const
{
   if (mLut.empty())
      return *wxBLACK;

   v = std::clamp(v, 0.0, 1.0);

   const double pos = v * (mLut.size() - 1);
   const int i = static_cast<int>(pos);
   const int j = std::min(i + 1, static_cast<int>(mLut.size() - 1));
   const double t = pos - i;

   const auto& c1 = mLut[i];
   const auto& c2 = mLut[j];

   return wxColour(
      static_cast<unsigned char>(c1.Red()   + t * (c2.Red()   - c1.Red())),
      static_cast<unsigned char>(c1.Green() + t * (c2.Green() - c1.Green())),
      static_cast<unsigned char>(c1.Blue()  + t * (c2.Blue()  - c1.Blue()))
   );
}