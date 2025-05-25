#include <Console.h>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QTcpSocket>

Console::Console(QObject* parent)
    : QObject(parent),
      sampleRate_(48000),
      appDataPath_(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Thetis/"),
      frequency_(7000000), // Initialize to 7 MHz
      rx1DSPMode_("2"), // Initialize to USB
      filterBandwidth_(3000) { // Initialize to 3000 Hz
    QDir().mkpath(appDataPath_);
    qDebug() << "Console constructor started";
    // Placeholder WDSP initialization
    qDebug() << "Console constructor finished";
}

Console::~Console() {
    qDebug() << "Console destructed";
}

void Console::setSampleRate(int rate) {
    sampleRate_ = rate;
    qDebug() << "Sample rate set to:" << rate;
}

int Console::getSampleRate() const {
    return sampleRate_;
}

QString Console::getRX1DSPMode() const {
    return rx1DSPMode_;
}

double Console::getVFOAFreq() const {
    return frequency_ / 1e6; // Return in MHz
}

QString Console::getAppDataPath() const {
    return appDataPath_;
}

void Console::setWavePlayback(bool enabled) {
    qDebug() << "Wave playback:" << (enabled ? "Enabled" : "Disabled");
    // Implement in Audio
}

void Console::setWaveRecord(bool enabled) {
    qDebug() << "Wave record:" << (enabled ? "Enabled" : "Disabled");
    // Implement in Audio
}

void Console::setWavePreamp(double gain) {
    qDebug() << "Wave preamp set to:" << gain;
    // Implement in Audio
}

void Console::setFrequency(qint64 freq) {
    if (freq >= 100000 && freq <= 30000000) { // Validate 0.1–30 MHz
        frequency_ = freq;
        qDebug() << "Frequency set to:" << freq << "Hz";
        // Placeholder: Future WDSP integration
        // SetRXAFrequency(channel(), freq);
    } else {
        qDebug() << "Invalid frequency:" << freq;
    }
}

void Console::setMode(const QString& mode) {
    QStringList validModes = {"1", "2", "3", "6"}; // LSB, USB, AM, CW
    if (validModes.contains(mode)) {
        rx1DSPMode_ = mode;
        qDebug() << "Radio mode set to:" << mode << "("
                 << (mode == "1" ? "LSB" : mode == "2" ? "USB" : mode == "3" ? "AM" : "CW") << ")";
        // Placeholder: Future WDSP integration
        // SetRXMode(channel(), mode.toInt());
    } else {
        qDebug() << "Invalid radio mode:" << mode;
    }
}

void Console::setFilterBandwidth(int bandwidth) {
    if (bandwidth >= 100 && bandwidth <= 10000) { // Validate 100–10000 Hz
        filterBandwidth_ = bandwidth;
        qDebug() << "Filter bandwidth set to:" << bandwidth << "Hz";
        // Placeholder: Future WDSP integration
        // SetRXFilter(channel(), bandwidth);
    } else {
        qDebug() << "Invalid filter bandwidth:" << bandwidth;
    }
}

void Console::tci_cmd(const QString& command, QTcpSocket* client) {
    qDebug() << "Received TCI command:" << command;
    // Placeholder: Implement TCI protocol parsing
    // Example: Parse commands like "frequency:7000000;" or "mode:USB;"
    QString response = "OK;" + command; // Placeholder response
    client->write(response.toUtf8());
}
