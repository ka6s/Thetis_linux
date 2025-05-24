#include <Console.h>
#include <wdsp.h>
#include <QDebug>

Console::Console(QObject* parent)
    : QObject(parent),
      channel_(0),
      sampleRate_(48000),
      audioMode_("CW"),
      frequency_(7000000),
      mode_("2") {
    qDebug() << "Console constructor started";
    initializeWDSP();
    qDebug() << "Console constructor finished";
}

Console::~Console() {
    qDebug() << "Console destructor started";
    destroy_anbEXT(channel_);
    qDebug() << "Console destructor finished";
}

void Console::initializeWDSP() {
    qDebug() << "Initializing WDSP for channel" << channel_;
    OpenChannel(channel_, 256, 256, sampleRate_, sampleRate_, sampleRate_, 0, 1, 0.0, 0.0, 0.0, 0.0, 0);
    qDebug() << "WDSP channel opened";
    create_anbEXT(channel_, 1, 256, sampleRate_, 0.0001, 0.1, 0.01, 0.0002, 0.001);
    qDebug() << "WDSP noise blanker created";
    SetRXAMode(channel_, 2);
    qDebug() << "WDSP mode set to USB";
}

void Console::tci_cmd(const QString& cmd, QTcpSocket* client) {
    qDebug() << "TCI command:" << cmd;
    QString response = "ACK:" + cmd;

    if (cmd.startsWith("FREQ")) {
        QString freqStr = cmd.mid(4);
        bool ok;
        qint64 freq = freqStr.toLongLong(&ok);
        if (ok) {
            setFrequency(freq);
            response = QString("FREQ:%1").arg(freq);
        }
    } else if (cmd.startsWith("MODE")) {
        QString mode = cmd.mid(4);
        setMode(mode);
        response = "MODE:" + mode;
    }

    client->write(response.toUtf8());
}

void Console::setCWTone(bool state) {
    qDebug() << "CW Tone:" << (state ? "On" : "Off");
}

void Console::setCWToneDuration(int duration) {
    qDebug() << "CW Tone Duration:" << duration << "ms";
}

void Console::setSampleRate(int rate) {
    sampleRate_ = rate;
    qDebug() << "Sample Rate:" << rate;
    SetInputSamplerate(channel_, rate);
    SetEXTANBSamplerate(channel_, rate);
}

void Console::setAudioMode(const QString& mode) {
    audioMode_ = mode;
    qDebug() << "Audio Mode:" << mode;
    int wdspMode = (mode == "CW" ? 6 : mode == "AM" ? 3 : 2);
    SetRXAMode(channel_, wdspMode);
}

void Console::setFrequency(qint64 freq) {
    frequency_ = freq;
    qDebug() << "Frequency:" << freq << "Hz";
}

void Console::setMode(const QString& mode) {
    mode_ = mode;
    qDebug() << "Mode:" << mode;
    int wdspMode = 2;
    if (mode == "1") wdspMode = 1;
    else if (mode == "3") wdspMode = 3;
    else if (mode == "6") wdspMode = 6;
    SetRXAMode(channel_, wdspMode);
}

void Console::setFilterBandwidth(int bandwidth) {
    qDebug() << "Filter Bandwidth:" << bandwidth << "Hz";
}
