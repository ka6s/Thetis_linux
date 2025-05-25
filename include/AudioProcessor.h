#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QObject>
#include <QString>
#include <portaudio.h>
#include <vector>

class AudioProcessor : public QObject {
    Q_OBJECT

public:
    explicit AudioProcessor(QObject* parent = nullptr);
    ~AudioProcessor();

    bool startPlayback(const QString& filename, int id);
    bool startRecording(const QString& filename, int channels, int sampleRate);
    void stopPlayback(int id);
    void stopRecording();
    void setPreamp(double gain);

    // Callback interface for PortAudio
    int processAudio(float* input, float* output, unsigned long frameCount);

private:
    struct WavHeader {
        char riff[4];
        uint32_t chunkSize;
        char wave[4];
        char fmt[4];
        uint32_t subchunk1Size;
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        char data[4];
        uint32_t subchunk2Size;
    };

    struct PlaybackState {
        FILE* file;
        WavHeader header;
        std::vector<float> buffer;
        size_t bufferPos;
        bool active;
        int id;
    };

    PlaybackState playback_;
    FILE* recordingFile_;
    WavHeader recordingHeader_;
    double preampGain_;
    bool writeWavHeader(FILE* file, int channels, int sampleRate);
    bool readWavHeader(FILE* file, WavHeader& header);
};

#endif // AUDIOPROCESSOR_H
