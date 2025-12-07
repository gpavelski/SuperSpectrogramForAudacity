#  SPDX-License-Identifier: GPL-2.0-or-later
#[[
A library for fast fourier transforms

Note -- this cannot be combined with lib-math, which depends on libsoxr, which
includes a different version of pffft.h
]]

add_library(FFTW3::fftw3 INTERFACE IMPORTED)
set_property(TARGET FFTW3::fftw3 PROPERTY
                    INTERFACE_INCLUDE_DIRECTORIES "C:/Program Files/FFTW3/"
)

set_property(TARGET FFTW3::fftw3 PROPERTY
                    INTERFACE_LINK_LIBRARIES 
                         "C:/Program Files/FFTW3/libfftw3-3.lib"
                         "C:/Program Files/FFTW3/libfftw3f-3.lib"
                         "C:/Program Files/FFTW3/libfftw3l-3.lib"
                  
)
               
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
   SuperSpectrumAnalyst.cpp
   SuperSpectrumAnalyst.h
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
