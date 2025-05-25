#include <Audio.h>
#include <Console.h>
#include <QDebug>
#include <cmath>

Audio::Audio(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      initialized_(false),
      stream_(nullptr),
      playbackEnabled_(false),
      preampGain_(1.0) {
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

    for (int i = 0; i < Pa_GetDeviceCount(); ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && QString(deviceInfo->name).contains("Jabra SPEAK 510 USB") && 
            deviceInfo->maxOutputChannels >= 2) {
            outputParameters.device = i;
            break;
        }
    }

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
        nullptr,
        &outputParameters,
        sampleRate,
        bufferSize,
        paClipOff,
        nullptr, // Muted
        nullptr
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

bool Audio::startPlayback(const QString& filename, int id) {
    qDebug() << "Starting playback for:" << filename << "ID:" << id;
    // Implement WAV file reading and playback
    return true; // Placeholder
}

bool Audio::startRecording(const QString& filename, int channels, int sampleRate) {
    qDebug() << "Starting recording to:" << filename << "Channels:" << channels << "SampleRate:" << sampleRate;
    // Implement WAV file writing
    return true; // Placeholder
}

void Audio::stopPlayback(int id) {
    qDebug() << "Stopping playback for ID:" << id;
}

void Audio::stopRecording() {
    qDebug() << "Stopping recording";
}

void Audio::setPlaybackEnabled(bool enabled) {
    playbackEnabled_ = enabled;
    qDebug() << "Playback enabled:" << enabled;
}

void Audio::setPreamp(double gain) {
    preampGain_ = gain;
    qDebug() << "Preamp gain set to:" << gain;
}

int Audio::audioCallback(const void* input, void* output, unsigned long frameCount,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags, void* userData) {
    // Implement playback/recording logic
    return paContinue;
}
