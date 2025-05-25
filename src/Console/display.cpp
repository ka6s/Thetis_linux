#include <Display.h>
#include <Console.h>
#include <NetworkIO.h>
#include <cmath>
#include <QDebug>

Display::Display(Console* console, NetworkIO* networkIO, QWidget* parent)
    : QWidget(parent),
      console_(console),
      networkIO_(networkIO),
      timer_(new QTimer(this)),
      updateCount_(0) {
    if (!console_) qDebug() << "Display: Warning: console is null";
    setMinimumSize(400, 200);
    spectrumData_.resize(256);
    connect(timer_, &QTimer::timeout, this, &Display::updateDisplay);
    if (networkIO_) {
        connect(networkIO_, &NetworkIO::iqDataReceived, this, &Display::updateSpectrumData);
    }
    timer_->start(50);
    qDebug() << "Display initialized with" << (networkIO_ ? "NetworkIO" : "simulated data");
}

Display::~Display() {
    qDebug() << "Display destructed";
}

void Display::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    // Draw spectrum
    painter.setPen(Qt::green);
    int width = this->width();
    int height = this->height();
    float xStep = static_cast<float>(width) / spectrumData_.size();

    // Only draw if spectrumData_ has valid values
    bool hasValidData = false;
    for (float val : spectrumData_) {
        if (!std::isnan(val) && !std::isinf(val)) {
            hasValidData = true;
            break;
        }
    }

    if (hasValidData) {
        for (int i = 0; i < spectrumData_.size() - 1; ++i) {
            int y1 = height - (spectrumData_[i] * height);
            int y2 = height - (spectrumData_[i + 1] * height);
            if (std::isnan(y1) || std::isinf(y1) || std::isnan(y2) || std::isinf(y2)) {
                qDebug() << "Invalid y coordinates at index" << i;
                continue;
            }
            painter.drawLine(i * xStep, y1, (i + 1) * xStep, y2);
        }
    } else {
        qDebug() << "No valid spectrum data to draw";
    }
}

void Display::updateDisplay() {
    // Generate simulated sine wave data as fallback
    for (int i = 0; i < spectrumData_.size(); ++i) {
        spectrumData_[i] = (sin(i * 0.1 + updateCount_ * 0.1) + 1) / 2.0;
    }
    updateCount_++;
    if (updateCount_ == 1) {
        qDebug() << "Display started with simulated sine wave";
    } else if (updateCount_ % 100 == 0) {
        qDebug() << "Display updated with simulated sine wave (" << updateCount_ << " updates)";
    }
    update();
}

void Display::updateSpectrumData(const QVector<float>& data) {
    if (data.size() == spectrumData_.size()) {
        spectrumData_ = data;
        update();
        qDebug() << "Updated spectrum data from NetworkIO";
    } else {
        qDebug() << "Invalid spectrum data size from NetworkIO:" << data.size();
    }
}
