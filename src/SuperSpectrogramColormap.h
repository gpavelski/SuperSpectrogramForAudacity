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
      if (m_lut.empty())
         return *wxBLACK;

      v = std::clamp(v, 0.0, 1.0);

      double pos = v * (m_lut.size() - 1);
      int i = static_cast<int>(pos);

      int j = std::min(i + 1, static_cast<int>(m_lut.size() - 1));
      double t = pos - i;

      const auto& c1 = m_lut[i];
      const auto& c2 = m_lut[j];

      return wxColour(
         static_cast<unsigned char>(c1.Red() + t * (c2.Red() - c1.Red())),
         static_cast<unsigned char>(c1.Green() + t * (c2.Green() - c1.Green())),
         static_cast<unsigned char>(c1.Blue() + t * (c2.Blue() - c1.Blue()))
      );
   }

   const std::vector<wxColour>& GetLUT() const override
   {
      return m_lut;
   }

protected:
   std::vector<wxColour> m_lut;
};

#endif
