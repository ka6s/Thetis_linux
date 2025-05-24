#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <portaudio.h>

class Console; // Forward declaration

class Audio : public QObject {
    Q_OBJECT

public:
    explicit Audio(Console* console, QObject* parent = nullptr);
    ~Audio();

    bool initialize(int sampleRate, int bufferSize);
    bool start();
    void stop();

private:
    static int audioCallback(const void* inputBuffer, void* outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userData);

    Console* console_;
    PaStream* stream_;
    int sampleRate_;
    int bufferSize_;
    bool initialized_;
};

#endif // AUDIO_H
