/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormap.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <wx/wx.h>
#include <wx/colour.h>

#include "Colormap/SuperSpectrogramColormapUtils.h"

class IColormap
{
public:
   virtual ~IColormap() = default;
   virtual wxColour Map(double v) const = 0;
   virtual wxString GetID() const = 0;
   virtual wxString GetName() const = 0;
};

class LUTColormap : public IColormap
{
public:
   LUTColormap(const wxString& id,
      const wxString& name,
      std::vector<wxColour> colors)
      : mId(id), mName(name), mLut(std::move(colors)) {
   }

   wxColour Map(double v) const override
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
         static_cast<unsigned char>(c1.Red() + t * (c2.Red() - c1.Red())),
         static_cast<unsigned char>(c1.Green() + t * (c2.Green() - c1.Green())),
         static_cast<unsigned char>(c1.Blue() + t * (c2.Blue() - c1.Blue()))
      );
   }

   wxString GetID() const override { return mId; }
   wxString GetName() const override { return mName; }

private:
   wxString mId;
   wxString mName;
   std::vector<wxColour> mLut;
};

template<const wxChar* ID, const wxChar* NAME, auto ColorFunc>
class LUTColormapGenerator : public LUTColormap
{
public:
   LUTColormapGenerator()
      : LUTColormap(ID, NAME, ColorFunc()) {
   }
};
