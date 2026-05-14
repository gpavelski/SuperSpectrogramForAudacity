/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramDataAdapter.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_DATA_ADAPTER__
#define __SUPER_SPECTROGRAM_DATA_ADAPTER__

#include <vector>
#include "SuperSpectrogramFrame.h"

class SuperSpectrogramDataAdapter
{
public:
   // Replaces internal dataset and recomputes derived representation
   void SetFrame(const SuperSpectrogramFrame& frame);

   // Clears all internal data and resets state
   void Clear();

   // Access to normalized spectrogram data (row-major flattened)
   const std::vector<float>& GetNormalized() const { return m_normalized; }

   size_t Rows() const { return m_rows; }
   size_t Cols() const { return m_cols; }

   double GetMaxFreq() const { return m_maxFreq; }
   size_t GetNumSamples() const { return m_numSamples; }

private:
   void Normalize();

private:
   std::vector<std::vector<double>> m_matrix;

   std::vector<float> m_normalized;

   size_t m_rows{ 0 };
   size_t m_cols{ 0 };

   double m_maxFreq{ 0.0 };
   size_t m_numSamples{ 0 };
};

#endif
