#  SPDX-License-Identifier: GPL-2.0-or-later
#[[
A library for fast fourier transforms

Note -- this cannot be combined with lib-math, which depends on libsoxr, which
includes a different version of pffft.h
]]

find_package(FFTW3 REQUIRED)

set( SOURCES
   Decimator.cpp
   Decimator.h
   FFT.cpp
   FFT.h
   FFTData.h
   FFTWContainer.cpp
   FFTWContainer.h
   PowerSpectrumGetter.cpp
   PowerSpectrumGetter.h
   RealFFTf.cpp
   RealFFTf.h
   Spectrum.cpp
   Spectrum.h
   SpectrumAnalyst.cpp
   SpectrumAnalyst.h
   SpectrumTransformer.cpp
   SpectrumTransformer.h
   STFTProcessor.cpp
   STFTProcessor.h
   SuperSpectrogramAnalyst.cpp
   SuperSpectrogramAnalyst.h
)

set( LIBRARIES
   pffft
   lib-math-interface
   lib-strings-interface
   lib-utility-interface
   FFTW3::fftw3
)
audacity_library( lib-fft "${SOURCES}" "${LIBRARIES}"
   "" ""
)
