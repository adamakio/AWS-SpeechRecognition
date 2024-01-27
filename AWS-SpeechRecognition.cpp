// main.cpp
#include <aws/core/Aws.h>
#include <aws/core/utils/threading/Semaphore.h>
#include <aws/transcribestreaming/TranscribeStreamingServiceClient.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionHandler.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionRequest.h>
#include <cstdio>

using namespace Aws;
using namespace Aws::TranscribeStreamingService;
using namespace Aws::TranscribeStreamingService::Model;

int SampleRate = 16000; // 16 Khz
int CaptureAudio(AudioStream& targetStream);

int main()
{
    std::cout << "We're in here" << std::endl;
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
    std::cout << "After InitAPI" << std::endl;
    {
        Aws::Client::ClientConfiguration config("adam");
        config.region = Aws::Region::US_EAST_1;
#ifdef _WIN32
        config.httpLibOverride = Aws::Http::TransferLibType::WIN_INET_CLIENT;
        config.caFile = "C:\\Users\\adam\\Downloads\\ca-bundle.crt";
#endif
        TranscribeStreamingServiceClient client(config);
        StartStreamTranscriptionHandler handler;
        handler.SetTranscriptEventCallback([](const TranscriptEvent& ev) {
            for (auto&& r : ev.GetTranscript().GetResults()) {
                if (r.GetIsPartial()) {
                    printf("[partial] ");
                }
                else {
                    printf("[Final] ");
                }
                for (auto&& alt : r.GetAlternatives()) {
                    printf("%s\n", alt.GetTranscript().c_str());
                }
            }
            });

        StartStreamTranscriptionRequest request;
        request.SetMediaSampleRateHertz(SampleRate);
        request.SetLanguageCode(LanguageCode::en_US);
        request.SetMediaEncoding(MediaEncoding::pcm);
        request.SetEventStreamHandler(handler);

        auto OnStreamReady = [](AudioStream& stream) {
            CaptureAudio(stream);
            stream.flush();
            stream.Close();
            };

        Aws::Utils::Threading::Semaphore signaling(0 /*initialCount*/, 1 /*maxCount*/);
        auto OnResponseCallback = [&signaling](const TranscribeStreamingServiceClient*,
            const Model::StartStreamTranscriptionRequest&,
            const Model::StartStreamTranscriptionOutcome&,
            const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) { signaling.Release(); };

        client.StartStreamTranscriptionAsync(request, OnStreamReady, OnResponseCallback, nullptr /*context*/);
        signaling.WaitOne(); // prevent the application from exiting until we're done
    }

    Aws::ShutdownAPI(options);

    return 0;
}
