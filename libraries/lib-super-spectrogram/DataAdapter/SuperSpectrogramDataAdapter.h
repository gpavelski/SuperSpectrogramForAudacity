/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramDataAdapter.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <limits>
#include <cmath>
#include <algorithm>
#include <vector>

#include "DataAdapter/SuperSpectrogramFrame.h"

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

   //
   // Analysis-domain accessors
   //
   double GetAnalysisMaxFreq() const
   {
      return m_analysisMaxFreq;
   }

   size_t GetAnalysisNumSamples() const
   {
      return m_analysisNumSamples;
   }

   //
   // Source-domain accessors
   //
   size_t GetOriginalNumSamples() const
   {
      return m_originalNumSamples;
   }

   double GetOriginalSampleRate() const
   {
      return m_originalSampleRate;
   }

private:
   void Normalize();

private:
   std::vector<std::vector<double>> m_matrix;

   std::vector<float> m_normalized;

   size_t m_rows{ 0 };
   size_t m_cols{ 0 };

   // Analysis-domain metadata
   double m_analysisMaxFreq{ 0.0 };
   size_t m_analysisNumSamples{ 0 };

   // Source-domain metadata
   size_t m_originalNumSamples{ 0 };
   double m_originalSampleRate{ 0.0 };
};

