/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormap.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_COLORMAP__
#define __SUPER_SPECTROGRAM_COLORMAP__

#include <wx/colour.h>
#include <vector>
#include <string>

class IColormap
{
public:
   virtual ~IColormap() = default;

   virtual wxString GetId() const = 0;
   virtual wxString GetDisplayName() const = 0;

   virtual wxColour Map(double normalizedValue) const = 0;

   virtual const std::vector<wxColour>& GetLUT() const = 0;
};

class LUTColormap : public IColormap
{
public:
   explicit LUTColormap(
      wxString id,
      wxString displayName,
      std::vector<wxColour> lut)
      :
      mId(std::move(id)),
      mDisplayName(std::move(displayName)),
      mLut(std::move(lut))
   {
   }

   wxString GetId() const override
   {
      return mId;
   }

   wxString GetDisplayName() const override
   {
      return mDisplayName;
   }

   wxColour Map(double v) const override;

   const std::vector<wxColour>& GetLUT() const override
   {
      return mLut;
   }

private:
   wxString mId;
   wxString mDisplayName;

protected:
   std::vector<wxColour> mLut;
};

#endif
