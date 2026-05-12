/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormap.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_COLORMAP__
#define __SUPER_SPECTROGRAM_COLORMAP__

#include <wx/colour.h>
#include <vector>

class IColormap
{
public:
   virtual ~IColormap() = default;

   // Map normalized value [0,1] → color
   virtual wxColour Map(double normalizedValue) const = 0;

   // Optional: precompute LUT if needed
   virtual const std::vector<wxColour>& GetLUT() const = 0;
};

class LUTColormap : public IColormap
{
public:
   explicit LUTColormap(std::vector<wxColour> lut)
      : m_lut(std::move(lut)) {}

   wxColour Map(double v) const override
   {
      int idx = static_cast<int>(v * (m_lut.size() - 1));
      idx = std::clamp(idx, 0, static_cast<int>(m_lut.size() - 1));
      return m_lut[idx];
   }

   const std::vector<wxColour>& GetLUT() const override
   {
      return m_lut;
   }

protected:
   std::vector<wxColour> m_lut;
};

#endif
