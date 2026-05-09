#include "SuperSpectrogramSession.h"

// Concrete includes (ONLY here)
#include "SuperSpectrogramController.h"
#include "SuperSpectrogramView.h"
#include "SuperSpectrogramAudioExtractor.h"
#include "SuperSpectrogramModel.h"
#include "SuperSpectrogramExportService.h"
#include "SuperSpectrogramConfig.h"

#include "ProjectWindows.h"   // GetProjectFrame
#include <wx/window.h>
#include <wx/event.h>

//------------------------------------------------------------
// Construction / Destruction
//------------------------------------------------------------
SuperSpectrogramSession::SuperSpectrogramSession(AudacityProject& project)
   : mProject(project)
{
}

SuperSpectrogramSession::~SuperSpectrogramSession()
{
   Destroy();
}

//------------------------------------------------------------
// Public API
//------------------------------------------------------------
void SuperSpectrogramSession::Show()
{
   if (mView) {
      mView->Raise();
      mView->SetFocus();
      return;
   }

   Create();
}

void SuperSpectrogramSession::SetAudio(Floats data, size_t len, double rate)
{
   mOwnedData = std::move(data);
   mData = mOwnedData.get();
   mLength = len;
   mRate = rate;
}

const float* SuperSpectrogramSession::GetAudioData() const
{
   return mData;
}

size_t SuperSpectrogramSession::GetAudioLength() const
{
   return mLength;
}

double SuperSpectrogramSession::GetSampleRate() const
{
   return mRate;
}

SuperSpectrogramConfig& SuperSpectrogramSession::GetConfig()
{
   return *mConfig;
}

const SuperSpectrogramConfig& SuperSpectrogramSession::GetConfig() const
{
   return *mConfig;
}

//------------------------------------------------------------
// Lifecycle
//------------------------------------------------------------
void SuperSpectrogramSession::Create()
{
   // --- View ---
   mView = new SuperSpectrogramView(
      &GetProjectFrame(mProject),
      wxID_ANY,
      XO("Super Spectrogram"),
      wxPoint{150, 150}
   );

   // --- Core services ---
   mAudioExtractor = std::make_unique<SuperSpectrogramAudioExtractor>(mProject);
   mExportService  = std::make_unique<SuperSpectrogramExportService>();
   mModel          = std::make_unique<SuperSpectrogramModel>();
   mConfig         = std::make_unique<SuperSpectrogramConfig>();

   // --- Controller ---
   mController = std::make_unique<SuperSpectrogramController>(
      *mAudioExtractor,
      *mExportService,
      *mModel,
      *this,
      *mView
   );

   mController->BindView();

   if (!mController->Initialize()) {
      Destroy();
      return;
   }

   // Lifecycle hook
   mView->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
      Destroy();
      evt.Skip();
   });

   mView->Show(true);
}

void SuperSpectrogramSession::Destroy()
{
   if (mView) {
      mView->Destroy();
      mView = nullptr;
   }

   mController.reset();
   mAudioExtractor.reset();
   mModel.reset();
   mExportService.reset();
   mConfig.reset();
}
