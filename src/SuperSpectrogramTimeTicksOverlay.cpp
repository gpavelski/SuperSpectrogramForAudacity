/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramTimeTicksOverlay.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramTimeTicksOverlay.h"
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramViewport.h"

#include <cmath>

//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
SuperSpectrogramTimeTicksOverlay::SuperSpectrogramTimeTicksOverlay(
    SuperSpectrogramConfig::TimeTickMode mode)
    : m_mode(mode)
{
}

//------------------------------------------------------------
// Public API
//------------------------------------------------------------
void SuperSpectrogramTimeTicksOverlay::SetMode(
    SuperSpectrogramConfig::TimeTickMode mode)
{
    m_mode = mode;
}

//------------------------------------------------------------
// Enable / Disable
//------------------------------------------------------------
void SuperSpectrogramTimeTicksOverlay::SetEnabled(bool enabled)
{
   m_enabled = enabled;
}

//------------------------------------------------------------
// Rendering
//------------------------------------------------------------
void SuperSpectrogramTimeTicksOverlay::Render(
    wxDC& dc,
    const wxSize& size,
    const SuperSpectrogramDataAdapter& data,
    const SuperSpectrogramViewport& viewport
) const
{
    if (m_mode == SuperSpectrogramConfig::TimeTickMode::None ||
        data.GetNumSamples() == 0 ||
        data.GetNormalized().empty())
        return;

    dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.SetTextForeground(*wxWHITE);

    const size_t numFrames = data.Cols();

    const double viewLeftFrame = viewport.Left();
    const double viewRightFrame = viewport.Right();

    const double frameSpan = viewRightFrame - viewLeftFrame;
    if (frameSpan <= 0)
        return;

    //------------------------------------------------------------
    // Mode: SECONDS
    //------------------------------------------------------------
    if (m_mode == SuperSpectrogramConfig::TimeTickMode::Seconds)
    {
        const double totalDuration =
            static_cast<double>(data.GetNumSamples()) /
            (2.0 * data.GetMaxFreq());

        const double viewLeftTime =
            (viewLeftFrame / numFrames) * totalDuration;

        const double viewRightTime =
            (viewRightFrame / numFrames) * totalDuration;

        const double viewWidthSec = viewRightTime - viewLeftTime;

        static const double tickSteps[] = {
            0.005, 0.01, 0.05, 0.1, 0.5, 1, 2, 5,
            10, 20, 30, 60, 120, 300, 600, 1800
        };

        const double targetPixelsPerTick = 80.0;
        const double secondsPerPixel = viewWidthSec / size.GetWidth();

        double step = tickSteps[0];
        for (double t : tickSteps)
        {
            if (t / secondsPerPixel >= targetPixelsPerTick)
            {
                step = t;
                break;
            }
        }

        double tick = std::ceil(viewLeftTime / step) * step;

        while (tick <= viewRightTime)
        {
            double fx =
                (tick - viewLeftTime) / viewWidthSec * size.GetWidth();

            wxString label;
            if (tick < 60.0)
            {
                label = wxString::Format("%g", tick);
            }
            else
            {
                label.Printf("%.0f:%02.0f",
                    std::floor(tick / 60.0),
                    std::fmod(tick, 60.0));
            }

            DrawTickLabel(dc, size, fx, label);

            tick += step;
        }
    }

    //------------------------------------------------------------
    // Mode: SAMPLES
    //------------------------------------------------------------
    else if (m_mode == SuperSpectrogramConfig::TimeTickMode::Samples)
    {
        const double samplesPerFrame =
            static_cast<double>(data.GetNumSamples()) / numFrames;

        const double viewLeftSample =
            viewLeftFrame * samplesPerFrame;

        const double viewRightSample =
            viewRightFrame * samplesPerFrame;

        const double viewWidthSamples =
            viewRightSample - viewLeftSample;

        static const double tickSteps[] = {
            1, 10, 50, 100, 500, 1000, 5000,
            10000, 50000, 100000, 500000, 1000000
        };

        const double targetPixelsPerTick = 80.0;
        const double samplesPerPixel =
            viewWidthSamples / size.GetWidth();

        double step = tickSteps[0];
        for (double t : tickSteps)
        {
            if (t / samplesPerPixel >= targetPixelsPerTick)
            {
                step = t;
                break;
            }
        }

        double tick = std::ceil(viewLeftSample / step) * step;

        while (tick <= viewRightSample)
        {
            double fx =
                (tick - viewLeftSample) / viewWidthSamples * size.GetWidth();

            wxString label = wxString::Format("%.0f", tick);

            DrawTickLabel(dc, size, fx, label);

            tick += step;
        }
    }
}

//------------------------------------------------------------
// Helpers
//------------------------------------------------------------
void SuperSpectrogramTimeTicksOverlay::DrawTickLabel(
    wxDC& dc,
    const wxSize& size,
    double fx,
    const wxString& label
) const
{
    wxCoord tw, th;
    dc.GetTextExtent(label, &tw, &th);

    int x = static_cast<int>(fx - tw / 2);
    int y = size.GetHeight() - th - 2;

    dc.SetBrush(*wxBLACK_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(x - 2, y - 1, tw + 4, th + 2);

    dc.SetPen(*wxWHITE_PEN);
    dc.DrawText(label, x, y);
}
