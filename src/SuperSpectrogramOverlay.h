/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramOverlay.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_OVERLAY__
#define __SUPER_SPECTROGRAM_OVERLAY__

#include <wx/dc.h>

class SuperSpectrogramDataAdapter;
class SuperSpectrogramViewport;

class ISpectrogramOverlay
{
public:
    virtual ~ISpectrogramOverlay() = default;

    virtual void Render(
        wxDC& dc,
        const wxSize& size,
        const SuperSpectrogramDataAdapter& data,
        const SuperSpectrogramViewport& viewport
    ) const = 0;

    virtual void SetEnabled(bool enabled) = 0;
    virtual bool IsEnabled() const = 0;
};

#endif
