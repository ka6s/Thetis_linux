#include <AudioProcessor.h>
#include <QDebug>
#include <cstring>

AudioProcessor::AudioProcessor(QObject* parent)
    : QObject(parent),
      playback_{nullptr, {}, {}, 0, false, -1},
      recordingFile_(nullptr),
      preampGain_(1.0) {
    qDebug() << "AudioProcessor initialized";
}

AudioProcessor::~AudioProcessor() {
    stopPlayback(playback_.id);
    stopRecording();
    qDebug() << "AudioProcessor destructed";
}

bool AudioProcessor::readWavHeader(FILE* file, WavHeader& header) {
    if (!file) return false;
    rewind(file);
    if (fread(&header, sizeof(WavHeader), 1, file) != 1) {
        qDebug() << "AudioProcessor: Failed to read WAV header";
        return false;
    }
    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0 ||
        strncmp(header.fmt, "fmt ", 4) != 0 || strncmp(header.data, "data", 4) != 0) {
        qDebug() << "AudioProcessor: Invalid WAV header";
        return false;
    }
    if (header.audioFormat != 1 || header.sampleRate != 48000 || header.bitsPerSample != 16) {
        qDebug() << "AudioProcessor: Unsupported WAV format (PCM, 48000 Hz, 16-bit required)";
        return false;
    }
    return true;
}

bool AudioProcessor::writeWavHeader(FILE* file, int channels, int sampleRate) {
    if (!file) return false;
    WavHeader header = {};
    std::memcpy(header.riff, "RIFF", 4); // Changed from strncpy
    std::memcpy(header.wave, "WAVE", 4); // Changed from strncpy
    std::memcpy(header.fmt, "fmt ", 4);  // Changed from strncpy
    std::memcpy(header.data, "data", 4); // Changed from strncpy
    header.subchunk1Size = 16;
    header.audioFormat = 1; // PCM
    header.numChannels = channels;
    header.sampleRate = sampleRate;
    header.bitsPerSample = 16;
    header.byteRate = sampleRate * channels * header.bitsPerSample / 8;
    header.blockAlign = channels * header.bitsPerSample / 8;
    header.subchunk2Size = 0; // Updated later
    header.chunkSize = 36 + header.subchunk2Size;
    rewind(file);
    if (fwrite(&header, sizeof(WavHeader), 1, file) != 1) {
        qDebug() << "AudioProcessor: Failed to write WAV header";
        return false;
    }
    return true;
}

bool AudioProcessor::startPlayback(const QString& filename, int id) {
    stopPlayback(playback_.id);

    FILE* file = fopen(filename.toStdString().c_str(), "rb");
    if (!file) {
        qDebug() << "AudioProcessor: Failed to open playback file:" << filename;
        return false;
    }

    WavHeader header;
    if (!readWavHeader(file, header)) {
        fclose(file);
        return false;
    }

    // Read initial buffer
    playback_.buffer.resize(1024 * header.numChannels); // Buffer for 1024 frames
    size_t read = fread(playback_.buffer.data(), sizeof(float), playback_.buffer.size(), file);
    if (read < playback_.buffer.size()) {
        playback_.buffer.resize(read);
    }

    playback_.file = file;
    playback_.header = header;
    playback_.bufferPos = 0;
    playback_.active = true;
    playback_.id = id;
    qDebug() << "AudioProcessor: Started playback for:" << filename << "ID:" << id
             << "Channels:" << header.numChannels << "SampleRate:" << header.sampleRate;
    return true;
}

bool AudioProcessor::startRecording(const QString& filename, int channels, int sampleRate) {
    stopRecording();

    FILE* file = fopen(filename.toStdString().c_str(), "wb");
    if (!file) {
        qDebug() << "AudioProcessor: Failed to open recording file:" << filename;
        return false;
    }

    if (!writeWavHeader(file, channels, sampleRate)) {
        fclose(file);
        return false;
    }

    recordingFile_ = file;
    recordingHeader_.numChannels = channels;
    recordingHeader_.sampleRate = sampleRate;
    recordingHeader_.bitsPerSample = 16;
    qDebug() << "AudioProcessor: Started recording to:" << filename
             << "Channels:" << channels << "SampleRate:" << sampleRate;
    return true;
}

void AudioProcessor::stopPlayback(int id) {
    if (playback_.active && playback_.file && playback_.id == id) {
        fclose(playback_.file);
        playback_.file = nullptr;
        playback_.buffer.clear();
        playback_.bufferPos = 0;
        playback_.active = false;
        playback_.id = -1;
        qDebug() << "AudioProcessor: Stopped playback for ID:" << id;
    }
}

void AudioProcessor::stopRecording() {
    if (recordingFile_) {
        // Update WAV header with data size
        long dataSize = ftell(recordingFile_) - sizeof(WavHeader);
        recordingHeader_.subchunk2Size = dataSize;
        recordingHeader_.chunkSize = 36 + dataSize;
        rewind(recordingFile_);
        fwrite(&recordingHeader_, sizeof(WavHeader), 1, recordingFile_);
        fclose(recordingFile_);
        recordingFile_ = nullptr;
        qDebug() << "AudioProcessor: Stopped recording";
    }
}

void AudioProcessor::setPreamp(double gain) {
    preampGain_ = gain;
    qDebug() << "AudioProcessor: Preamp gain set to:" << gain;
}

int AudioProcessor::processAudio(float* input, float* output, unsigned long frameCount) {
    if (!output) return 0;

    // Playback
    if (playback_.active && playback_.file) {
        size_t framesNeeded = frameCount * playback_.header.numChannels;
        size_t framesAvailable = playback_.buffer.size() - playback_.bufferPos;
        if (framesAvailable < framesNeeded) {
            // Refill buffer
            memmove(playback_.buffer.data(), playback_.buffer.data() + playback_.bufferPos, framesAvailable * sizeof(float));
            playback_.buffer.resize(framesAvailable + 1024 * playback_.header.numChannels);
            size_t read = fread(playback_.buffer.data() + framesAvailable, sizeof(float),
                               playback_.buffer.size() - framesAvailable, playback_.file);
            playback_.buffer.resize(framesAvailable + read);
            playback_.bufferPos = 0;
            framesAvailable = playback_.buffer.size();
        }

        for (unsigned long i = 0; i < framesNeeded && i < framesAvailable; ++i) {
            output[i] += playback_.buffer[playback_.bufferPos + i] * preampGain_;
        }
        playback_.bufferPos += framesNeeded;
        if (playback_.bufferPos >= playback_.buffer.size()) {
            stopPlayback(playback_.id); // End of file
        }
    }

    // Recording
    if (recordingFile_ && input) {
        fwrite(input, sizeof(float), frameCount * recordingHeader_.numChannels, recordingFile_);
    }

    return 0; // Continue processing
}
