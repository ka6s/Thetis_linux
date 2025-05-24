#include <Display.h>
#include <Console.h>
#include <cmath>
#include <QDebug>

Display::Display(Console* console, QWidget* parent)
    : QWidget(parent),
      console_(console),
      timer_(new QTimer(this)) {
    setMinimumSize(400, 200);
    spectrumData_.resize(256);
    connect(timer_, &QTimer::timeout, this, &Display::updateDisplay);
    timer_->start(50);
    qDebug() << "Display initialized";
}

Display::~Display() {
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

    for (int i = 0; i < spectrumData_.size() - 1; ++i) {
        int y1 = height - (spectrumData_[i] * height);
        int y2 = height - (spectrumData_[i + 1] * height);
        painter.drawLine(i * xStep, y1, (i + 1) * xStep, y2);
    }
}

void Display::updateDisplay() {
    // Placeholder: Simulated spectrum data
    for (int i = 0; i < spectrumData_.size(); ++i) {
        spectrumData_[i] = (sin(i * 0.1) + 1) / 2.0;
    }
    update();
}
