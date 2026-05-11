/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramSession.cpp

  Guilherme Pavelski

**********************************************************************/

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

bool SuperSpectrogramSession::InitializeAudio()
{
   auto result = mAudioExtractor->Extract();

   if (result.status != SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::Success)
      return false;

   mAudio = AudioData{
      std::move(*result.data),
      result.length,
      result.rate
   };

   return true;
}

const SuperSpectrogramSession::AudioData& SuperSpectrogramSession::GetAudio() const
{
   return *mAudio;
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
   // --- Core services ---
   mAudioExtractor = std::make_unique<SuperSpectrogramAudioExtractor>(mProject);
   mExportService = std::make_unique<SuperSpectrogramExportService>();
   mModel = std::make_unique<SuperSpectrogramModel>();
   mConfig = std::make_unique<SuperSpectrogramConfig>();

   mConfig->Load();

   // ------------------------------------------------------------
   // Resolve window geometry from config
   // ------------------------------------------------------------
   wxSize size(
      SuperSpectrogramConstants::UI::kDefaultWidth,
      SuperSpectrogramConstants::UI::kDefaultHeight
   );

   wxPoint pos{ 150, 150 };

   if (mConfig->window.valid)
   {
      size = wxSize(
         mConfig->window.width,
         mConfig->window.height
      );

      if (mConfig->window.posX != wxDefaultCoord &&
         mConfig->window.posY != wxDefaultCoord)
      {
         pos = wxPoint(
            mConfig->window.posX,
            mConfig->window.posY
         );
      }
   }

   // --- View ---
   mView = new SuperSpectrogramView(
      &GetProjectFrame(mProject),
      wxID_ANY,
      XO("Super Spectrogram"),
      pos
   );

   mView->SetSize(size);

   // ------------------------------------------------------------
   // Persist window geometry on resize
   // ------------------------------------------------------------
   mView->Bind(wxEVT_SIZE, [this](wxSizeEvent& evt) {
      if (!mView || !mConfig)
         return;

      // Avoid saving maximized state as raw size
      if (!mView->IsMaximized())
      {
         auto size = mView->GetSize();
         auto pos = mView->GetPosition();

         auto& win = mConfig->window;
         win.width = size.GetWidth();
         win.height = size.GetHeight();
         win.posX = pos.x;
         win.posY = pos.y;
         win.valid = true;

         mConfig->Save();
      }

      evt.Skip();
      });

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

   // ------------------------------------------------------------
   // Save geometry on close as well (final snapshot)
   // ------------------------------------------------------------
   mView->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
      if (mView && mConfig && !mView->IsMaximized())
      {
         auto size = mView->GetSize();
         auto pos = mView->GetPosition();

         auto& win = mConfig->window;
         win.width = size.GetWidth();
         win.height = size.GetHeight();
         win.posX = pos.x;
         win.posY = pos.y;
         win.valid = true;

         mConfig->Save();
      }

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

void SuperSpectrogramSession::SaveWindowGeometry()
{
   if (!mView || !mConfig)
      return;

   auto size = mView->GetSize();
   auto pos = mView->GetPosition();

   auto& win = mConfig->window;
   win.width = size.GetWidth();
   win.height = size.GetHeight();
   win.posX = pos.x;
   win.posY = pos.y;
   win.valid = true;

   mConfig->Save();
}
