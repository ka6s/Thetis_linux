#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <portaudio.h>
#include <QString>

class Console;

class Audio : public QObject {
    Q_OBJECT

public:
    explicit Audio(Console* console, QObject* parent = nullptr);
    ~Audio();

    bool initialize(int sampleRate, int bufferSize);
    void start();
    void stop();
    bool startPlayback(const QString& filename, int id);
    bool startRecording(const QString& filename, int channels, int sampleRate);
    void stopPlayback(int id);
    void stopRecording();
    void setPlaybackEnabled(bool enabled);
    void setPreamp(double gain);

private:
    static int audioCallback(const void* input, void* output, unsigned long frameCount,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags, void* userData);

    Console* console_;
    bool initialized_;
    void* stream_;
    bool playbackEnabled_;
    double preampGain_;
    // Add playback/recording state (implemented later)
};

#endif // AUDIO_H
