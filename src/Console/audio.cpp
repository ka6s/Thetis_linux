#include <Audio.h>
#include <Console.h>
#include <wdsp.h>
#include <QDebug>
#include <string.h>

Audio::Audio(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      stream_(nullptr),
      sampleRate_(48000),
      bufferSize_(256),
      initialized_(false) {
    Pa_Initialize();
}

Audio::~Audio() {
    stop();
    Pa_Terminate();
}

bool Audio::initialize(int sampleRate, int bufferSize) {
    sampleRate_ = sampleRate;
    bufferSize_ = bufferSize;

    // Defer WDSP receiver initialization to Console
    console_->setSampleRate(sampleRate_);

    // Set up PortAudio stream
    PaError err;
    PaStreamParameters inputParams, outputParams;

    inputParams.device = Pa_GetDefaultInputDevice();
    if (inputParams.device == paNoDevice) {
        qDebug() << "No input device available";
        return false;
    }
    inputParams.channelCount = 2; // Stereo (I/Q)
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device == paNoDevice) {
        qDebug() << "No output device available";
        return false;
    }
    outputParams.channelCount = 2; // Stereo (I/Q)
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream_, &inputParams, &outputParams, sampleRate_, bufferSize_,
                        paNoFlag, audioCallback, this);
    if (err != paNoError) {
        qDebug() << "PortAudio error:" << Pa_GetErrorText(err);
        return false;
    }

    initialized_ = true;
    return true;
}

bool Audio::start() {
    if (!initialized_) {
        qDebug() << "Audio not initialized";
        return false;
    }
    PaError err = Pa_StartStream(stream_);
    if (err != paNoError) {
        qDebug() << "PortAudio start error:" << Pa_GetErrorText(err);
        return false;
    }
    qDebug() << "Audio stream started";
    return true;
}

void Audio::stop() {
    if (stream_) {
        Pa_StopStream(stream_);
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }
    // Defer WDSP cleanup to Console
    console_->setAudioMode("STOP");
}

int Audio::audioCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData) {
    float* in = (float*)inputBuffer;
    float* out = (float*)outputBuffer;
    int error = 0;

    // Process audio with WDSP (channel 0)
    if (inputBuffer && outputBuffer) {
        // Split stereo input/output into I/Q buffers
        float* Iin = new float[framesPerBuffer];
        float* Qin = new float[framesPerBuffer];
        float* Iout = new float[framesPerBuffer];
        float* Qout = new float[framesPerBuffer];

        // Interleave stereo: in[0,2,4,...] = I, in[1,3,5,...] = Q
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            Iin[i] = in[2 * i];
            Qin[i] = in[2 * i + 1];
        }

        // WDSP processing
        fexchange2(0, Iin, Qin, Iout, Qout, &error);
        if (error != 0) {
            qDebug() << "WDSP fexchange2 error:" << error;
        }

        // Recombine I/Q into stereo output
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            out[2 * i] = Iout[i];
            out[2 * i + 1] = Qout[i];
        }

        delete[] Iin;
        delete[] Qin;
        delete[] Iout;
        delete[] Qout;
    } else if (outputBuffer) {
        // Zero output if no input
        memset(out, 0, framesPerBuffer * 2 * sizeof(float));
    }

    return paContinue;
}
