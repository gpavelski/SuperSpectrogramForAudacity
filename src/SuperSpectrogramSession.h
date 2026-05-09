/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramSession.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_SESSION__
#define __SUPER_SPECTROGRAM_SESSION__

#include <memory>

// Forward declarations (avoid heavy includes in header)
class SuperSpectrogramController;
class SuperSpectrogramView;
class SuperSpectrogramAudioExtractor;
class SuperSpectrogramModel;
class SuperSpectrogramExportService;
class SuperSpectrogramConfig;
class AudacityProject;

class SuperSpectrogramSession
{
public:
   explicit SuperSpectrogramSession(AudacityProject& project);
   ~SuperSpectrogramSession();

   // UI entry point
   void Show();

   // Audio ownership (Session = source of truth)
   void SetAudio(
      Floats data,
      size_t len,
      double rate
   );

   const float* GetAudioData() const;
   size_t GetAudioLength() const;
   double GetSampleRate() const;

   SuperSpectrogramConfig& GetConfig();
   const SuperSpectrogramConfig& GetConfig() const;

private:
   void Create();
   void Destroy();

private:
   AudacityProject& mProject;

   // View (owned, raw pointer due to wx lifetime rules)
   SuperSpectrogramView* mView = nullptr;

   // Core components
   std::unique_ptr<SuperSpectrogramController> mController;
   std::unique_ptr<SuperSpectrogramAudioExtractor> mAudioExtractor;
   std::unique_ptr<SuperSpectrogramModel> mModel;
   std::unique_ptr<SuperSpectrogramExportService> mExportService;

   // Audio state
   Floats mOwnedData;
   const float* mData = nullptr;
   size_t mLength = 0;
   double mRate = 0.0;

   // Persistent config (owned here now)
   std::unique_ptr<SuperSpectrogramConfig> mConfig;
};

#endif // __SUPER_SPECTROGRAM_SESSION__
