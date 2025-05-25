#include <Radio.h>
#include <Console.h>
#include <QDebug>

Radio::Radio(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      hardwareType_("HPSDR"), // Default hardware
      initialized_(false) {
    qDebug() << "Radio initialized";
}

Radio::~Radio() {
    stop();
    qDebug() << "Radio destructed";
}

bool Radio::initialize() {
    if (!console_) {
        qDebug() << "Radio: Console not set";
        return false;
    }

    // Placeholder: Initialize radio hardware (e.g., HPSDR, Hermes)
    int sampleRate = console_->getSampleRate();
    qDebug() << "Radio: Initializing with sample rate:" << sampleRate;

    // Placeholder: WDSP initialization
    // Example: OpenChannel(console_->channel(), sampleRate, ...);
    initialized_ = true;
    return true;
}

void Radio::setFrequency(qint64 freq, bool vfoA) {
    if (!initialized_) {
        qDebug() << "Radio: Not initialized";
        return;
    }
    if (freq >= 100000 && freq <= 30000000) { // Validate 0.1–30 MHz
        qDebug() << "Radio: Setting" << (vfoA ? "VFO-A" : "VFO-B") << "frequency to:" << freq << "Hz";
        // Placeholder: WDSP frequency setting
        // SetRXAFrequency(console_->channel(), freq);
        emit frequencyChanged(freq);
    } else {
        qDebug() << "Radio: Invalid frequency:" << freq;
    }
}

void Radio::setMode(const QString& mode) {
    if (!initialized_) {
        qDebug() << "Radio: Not initialized";
        return;
    }
    QStringList validModes = {"LSB", "USB", "AM", "CW"}; // Simplified from radio.cs
    if (validModes.contains(mode, Qt::CaseInsensitive)) {
        qDebug() << "Radio: Setting mode to:" << mode;
        // Placeholder: WDSP mode setting
        // SetRXMode(console_->channel(), mode);
        emit modeChanged(mode);
    } else {
        qDebug() << "Radio: Invalid mode:" << mode;
    }
}

void Radio::setFilter(int low, int high) {
    if (!initialized_) {
        qDebug() << "Radio: Not initialized";
        return;
    }
    if (low < high && low >= -10000 && high <= 10000) { // Validate filter range
        qDebug() << "Radio: Setting filter low:" << low << "high:" << high;
        // Placeholder: WDSP filter setting
        // SetRXFilter(console_->channel(), low, high);
    } else {
        qDebug() << "Radio: Invalid filter settings: low=" << low << "high=" << high;
    }
}

void Radio::start() {
    if (!initialized_) {
        qDebug() << "Radio: Not initialized";
        return;
    }
    qDebug() << "Radio: Starting";
    // Placeholder: Start radio hardware and DSP
}

void Radio::stop() {
    if (initialized_) {
        qDebug() << "Radio: Stopping";
        // Placeholder: Stop radio hardware and DSP
        // CloseChannel(console_->channel());
        initialized_ = false;
    }
}

QString Radio::getHardwareType() const {
    return hardwareType_;
}
