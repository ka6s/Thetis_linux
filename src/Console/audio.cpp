#include <Audio.h>
#include <Console.h>
#include <portaudio.h>
#include <QDebug>
#include <cmath>

Audio::Audio(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      initialized_(false),
      stream_(nullptr),
      phase_(0.0) {
    qDebug() << "Audio initialized";
}

Audio::~Audio() {
    stop();
}

bool Audio::initialize(int sampleRate, int bufferSize) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        qDebug() << "PortAudio initialization failed:" << Pa_GetErrorText(err);
        return false;
    }

    PaStreamParameters outputParameters;
    outputParameters.device = paNoDevice;

    // Prefer Jabra SPEAK 510 USB (Card 4, Device 0)
    for (int i = 0; i < Pa_GetDeviceCount(); ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && QString(deviceInfo->name).contains("Jabra SPEAK 510 USB") && 
            deviceInfo->maxOutputChannels >= 2) {
            outputParameters.device = i;
            break;
        }
    }

    // Fallback to default device
    if (outputParameters.device == paNoDevice) {
        outputParameters.device = Pa_GetDefaultOutputDevice();
        if (outputParameters.device == paNoDevice) {
            qDebug() << "No default output device found";
            Pa_Terminate();
            return false;
        }
    }

    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(outputParameters.device);
    qDebug() << "Selected audio device:" << deviceInfo->name
             << "Host API:" << Pa_GetHostApiInfo(deviceInfo->hostApi)->name;

    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
        &stream_,
        nullptr, // No input
        &outputParameters,
        sampleRate,
        bufferSize,
        paClipOff,
        nullptr, // Muted: no callback
        nullptr  // No user data
    );
    if (err != paNoError) {
        qDebug() << "Failed to open PortAudio stream:" << Pa_GetErrorText(err);
        Pa_Terminate();
        return false;
    }

    initialized_ = true;
    qDebug() << "Audio initialized with sample rate:" << sampleRate << "buffer size:" << bufferSize;
    return true;
}

int Audio::audioCallback(const void* input, void* output, unsigned long frameCount,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags, void* userData) {
    Audio* audio = static_cast<Audio*>(userData);
    float* out = static_cast<float*>(output);
    const double frequency = 440.0;
    const double sampleRate = 48000.0;
    const double amplitude = 0.5;

    for (unsigned long i = 0; i < frameCount; ++i) {
        float sample = amplitude * sin(audio->phase_);
        out[i * 2] = sample;
        out[i * 2 + 1] = sample;
        audio->phase_ += 2.0 * M_PI * frequency / sampleRate;
        if (audio->phase_ > 2.0 * M_PI) {
            audio->phase_ -= 2.0 * M_PI;
        }
    }

    return paContinue;
}

void Audio::start() {
    if (!initialized_ || !stream_) {
        qDebug() << "Audio not initialized or stream is null, cannot start";
        return;
    }
    PaError err = Pa_StartStream(stream_);
    if (err != paNoError) {
        qDebug() << "Failed to start PortAudio stream:" << Pa_GetErrorText(err);
        return;
    }
    qDebug() << "Audio stream started";
}

void Audio::stop() {
    if (initialized_ && stream_) {
        Pa_CloseStream(stream_);
        Pa_Terminate();
        initialized_ = false;
        stream_ = nullptr;
        qDebug() << "Audio stream stopped";
    }
}
