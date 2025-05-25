#include <Display.h>
#include <Console.h>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QDebug>
#include <cmath>
#include <algorithm>

SpectrumWidget::SpectrumWidget(QWidget* parent)
    : QWidget(parent),
      centerFrequency_(14.0e6),
      bandwidth_(96000) {
    setMinimumSize(400, 200);
}

void SpectrumWidget::setData(const std::vector<float>& data, double centerFreq, int bandwidth) {
    spectrumData_.resize(data.size());
    // Shift and scale to -120 to 0 dB
    if (!data.empty()) {
        float minVal = *std::min_element(data.begin(), data.end());
        float maxVal = *std::max_element(data.begin(), data.end());
        float offset = maxVal; // Shift to make max 0 dB
        for (size_t i = 0; i < data.size(); ++i) {
            spectrumData_[i] = data[i] - offset;
            spectrumData_[i] = std::max(-120.0f, std::min(0.0f, spectrumData_[i]));
        }
        qDebug() << "SpectrumWidget: Raw data min:" << minVal << "max:" << maxVal;
    } else {
        spectrumData_.clear();
    }
    centerFrequency_ = centerFreq;
    bandwidth_ = bandwidth;
    if (!spectrumData_.empty()) {
        auto [minIt, maxIt] = std::minmax_element(spectrumData_.begin(), spectrumData_.end());
        qDebug() << "SpectrumWidget: Data size:" << spectrumData_.size()
                 << "min:" << *minIt << "max:" << *maxIt;
    } else {
        qDebug() << "SpectrumWidget: Received empty data";
    }
    update();
}

void SpectrumWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    if (spectrumData_.empty() || spectrumData_.size() < 2) {
        std::vector<float> simData(1024, -80.0f);
        for (size_t i = 0; i < simData.size(); ++i) {
            simData[i] = -80.0f + 40.0f * (sin(2 * M_PI * i / simData.size()) + 1.0f) / 2.0f;
        }
        spectrumData_ = simData;
        qDebug() << "SpectrumWidget: Using simulated spectrum data";
    }

    painter.setPen(Qt::lightGray);
    int width = this->width();
    int height = this->height();
    for (int x = 0; x <= width; x += width / 10) {
        painter.drawLine(x, 0, x, height);
    }
    for (int y = 0; y <= height; y += height / 6) {
        painter.drawLine(0, y, width, y);
    }

    painter.setPen(QPen(Qt::blue, 2));
    QPainterPath path;
    double xStep = static_cast<double>(width) / (spectrumData_.size() - 1);
    for (size_t i = 0; i < spectrumData_.size(); ++i) {
        float y = spectrumData_[i];
        int yPos = static_cast<int>((0.0f - y) / 120.0f * height); // Adjusted for -120 dB
        yPos = std::max(0, std::min(height, yPos));
        double x = i * xStep;
        if (i == 0) {
            path.moveTo(x, yPos);
        } else {
            path.lineTo(x, yPos);
        }
        if (i < 20 || i > spectrumData_.size() - 20 || y > -20) {
            qDebug() << "SpectrumWidget: Plot point[" << i << "] x:" << x << "y:" << yPos << "dB:" << y;
        }
    }

    painter.drawPath(path);

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8));
    double freqMin = centerFrequency_ - bandwidth_ / 2.0;
    double freqMax = centerFrequency_ + bandwidth_ / 2.0;
    painter.drawText(QRectF(0, height - 20, 60, 20),
                     QString("%1 MHz").arg(freqMin / 1e6, 0, 'f', 3));
    painter.drawText(QRectF(width - 60, height - 20, 60, 20),
                     QString("%1 MHz").arg(freqMax / 1e6, 0, 'f', 3));
    painter.drawText(QRectF(0, 0, 50, 20), "0 dB");
    painter.drawText(QRectF(0, height - 20, 50, 20), "-120 dB");
}

void SpectrumWidget::mousePressEvent(QMouseEvent* event) {
    double x = event->pos().x();
    double width = static_cast<double>(this->width());
    double freq = centerFrequency_ - bandwidth_ / 2.0 + (x / width) * bandwidth_;
    emit frequencySelected(freq);
    qDebug() << "SpectrumWidget: Frequency selected at" << freq << "Hz";
}

Display::Display(Console* palette, QWidget* parent)
    : QWidget(parent),
      palette_(palette),
      spectrumWidget_(new SpectrumWidget(this)),
      centerFrequency_(14.0e6),
      bandwidth_(96000) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(spectrumWidget_);
    setLayout(layout);

    connect(spectrumWidget_, &SpectrumWidget::frequencySelected,
            this, &Display::frequencyChanged);

    qDebug() << "Display initialized with center frequency:" << centerFrequency_
             << "Hz, bandwidth:" << bandwidth_ << "Hz";
}

Display::~Display() {
    qDebug() << "Display destroyed";
}

void Display::setCenterFrequency(double freq) {
    centerFrequency_ = freq;
    spectrumWidget_->setData(spectrumData_, centerFrequency_, bandwidth_);
    qDebug() << "Display: Center frequency set to" << freq << "Hz";
}

void Display::setBandwidth(int bw) {
    bandwidth_ = bw;
    spectrumWidget_->setData(spectrumData_, centerFrequency_, bandwidth_);
    qDebug() << "Display: Bandwidth set to" << bw << "Hz";
}

void Display::updateSpectrum(const float* data, int size) {
    qDebug() << "Display: Received spectrum data, size:" << size;
    if (size <= 0) {
        qDebug() << "Display: Invalid spectrum data size";
        return;
    }
    spectrumData_.assign(data, data + size);
    spectrumWidget_->setData(spectrumData_, centerFrequency_, bandwidth_);
    qDebug() << "Display: Spectrum updated with" << size << "points";
}
