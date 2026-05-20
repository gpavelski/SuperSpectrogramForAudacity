# SPDX-License-Identifier: GPL-2.0-or-later

# ---------------------------------------------
# Dependencies
# ---------------------------------------------
find_package(FFTW3 REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base)

# ---------------------------------------------
# Source groups for modularity
# ---------------------------------------------
set(AUDIO_EXTRACTOR_SOURCES
    AudioExtractor/SuperSpectrogramAudioExtractor.cpp
    AudioExtractor/SuperSpectrogramAudioExtractor.h
)

set(CONFIG_SOURCES
    Config/SuperSpectrogramConfig.cpp
    Config/SuperSpectrogramConfig.h
    Config/SuperSpectrogramConstants.h
    Config/SuperSpectrogramUIOptions.cpp
    Config/SuperSpectrogramUIOptions.h
)

set(CONTROLLER_SOURCES
    Controller/SuperSpectrogramController.cpp
    Controller/SuperSpectrogramController.h
    Controller/SuperSpectrogramInteractionController.cpp
    Controller/SuperSpectrogramInteractionController.h
)

set(DATA_ADAPTER_SOURCES
    DataAdapter/SuperSpectrogramDataAdapter.cpp
    DataAdapter/SuperSpectrogramDataAdapter.h
    DataAdapter/SuperSpectrogramExportService.cpp
    DataAdapter/SuperSpectrogramExportService.h
    DataAdapter/SuperSpectrogramFrame.h
)

set(MODEL_SOURCES
    Model/SuperSpectrogramModel.cpp
    Model/SuperSpectrogramModel.h
    Model/Cheby1LowPassIIRFilter.h
    Model/Decimator.cpp
    Model/Decimator.h
    Model/FFTData.h
    Model/FFTWContainer.cpp
    Model/FFTWContainer.h
    Model/STFTProcessor.cpp
    Model/STFTProcessor.h
)

set(OVERLAY_SOURCES
    Overlay/SuperSpectrogramNotesLinesOverlay.cpp
    Overlay/SuperSpectrogramNotesLinesOverlay.h
    Overlay/SuperSpectrogramTimeTicksOverlay.cpp
    Overlay/SuperSpectrogramTimeTicksOverlay.h
    Overlay/SuperSpectrogramOverlay.h
)

set(VIEW_SOURCES
    View/SuperSpectrogramPanel.cpp
    View/SuperSpectrogramPanel.h
    View/SuperSpectrogramRenderer.cpp
    View/SuperSpectrogramRenderer.h
    View/SuperSpectrogramRenderContext.h
    View/SuperSpectrogramSession.cpp
    View/SuperSpectrogramSession.h
    View/SuperSpectrogramView.cpp
    View/SuperSpectrogramView.h
    View/SuperSpectrogramViewModel.cpp
    View/SuperSpectrogramViewModel.h
    View/SuperSpectrogramViewport.cpp
    View/SuperSpectrogramViewport.h
    View/SuperSpectrogramMouseEvent.h
)

set(COLORMAP_SOURCES
    Colormap/SuperSpectrogramColormap.h
    Colormap/SuperSpectrogramColormapRegistry.cpp
    Colormap/SuperSpectrogramColormapRegistry.h
    Colormap/SuperSpectrogramColormapUtils.h
    Colormap/SuperSpectrogramColormapRegistration.h
    Colormap/Colormaps/SuperSpectrogramCividisColormap.cpp
    Colormap/Colormaps/SuperSpectrogramGrayColormap.cpp
    Colormap/Colormaps/SuperSpectrogramHotColormap.cpp
    Colormap/Colormaps/SuperSpectrogramInfernoColormap.cpp
    Colormap/Colormaps/SuperSpectrogramJetColormap.cpp
    Colormap/Colormaps/SuperSpectrogramMagmaColormap.cpp
    Colormap/Colormaps/SuperSpectrogramParulaColormap.cpp
    Colormap/Colormaps/SuperSpectrogramViridisColormap.cpp
)

# ---------------------------------------------
# Merge all sources
# ---------------------------------------------
set(SOURCES
    ${AUDIO_EXTRACTOR_SOURCES}
    ${CONFIG_SOURCES}
    ${CONTROLLER_SOURCES}
    ${DATA_ADAPTER_SOURCES}
    ${MODEL_SOURCES}
    ${OVERLAY_SOURCES}
    ${VIEW_SOURCES}
    ${COLORMAP_SOURCES}
)

# ---------------------------------------------
# Link dependencies
# ---------------------------------------------
set(LIBRARIES
    FFTW3::fftw3
    lib-basic-ui-interface
    lib-math-interface
    lib-menus-interface
    lib-preferences-interface
    lib-time-frequency-selection-interface
    lib-utility-interface
    lib-wave-track-interface
    lib-wx-wrappers-interface
    wxwidgets::wxwidgets
)

audacity_library( lib-super-spectrogram "${SOURCES}" "${LIBRARIES}"
   "" ""
)