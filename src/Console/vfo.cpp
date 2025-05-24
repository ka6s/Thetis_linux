#include <VFO.h>
#include <Console.h>
#include <wdsp.h>
#include <QDebug>

VFO::VFO(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      frequency_(7000000),
      vfoMode_("VFO A"),
      vfoModes_({"VFO A", "VFO B", "Split"}),
      stepSize_(100) {
    qDebug() << "VFO initialized with frequency:" << frequency_ << "Hz, mode:" << vfoMode_;
}

VFO::~VFO() {
}

void VFO::setFrequency(qint64 freq) {
    frequency_ = freq;
    console_->setFrequency(freq);
    qDebug() << "VFO frequency set to:" << freq << "Hz";
    // Placeholder: Future WDSP integration
    // SetRXAFrequency(console_->channel(), freq);
}

qint64 VFO::getFrequency() const {
    return frequency_;
}

void VFO::setVFOMode(const QString& mode) {
    if (vfoModes_.contains(mode)) {
        vfoMode_ = mode;
        qDebug() << "VFO mode set to:" << mode;
        // Placeholder: Configure WDSP for VFO mode (e.g., Split)
    } else {
        qDebug() << "Invalid VFO mode:" << mode;
    }
}

QStringList VFO::getVFOModes() const {
    return vfoModes_;
}

void VFO::setStepSize(int step) {
    stepSize_ = step;
    qDebug() << "VFO step size set to:" << step << "Hz";
}

int VFO::getStepSize() const {
    return stepSize_;
}
