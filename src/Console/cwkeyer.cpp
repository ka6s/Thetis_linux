#include <CWKeyer.h>
#include <Console.h>
#include <QDebug>

CWKeyer::CWKeyer(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      iambic_(false),
      speedWpm_(20),
      breakInEnabled_(false),
      breakInDelay_(50),
      keyState_(false) {
    keyerTimer_ = new QTimer(this);
    connect(keyerTimer_, &QTimer::timeout, this, &CWKeyer::onKeyerTimer);
}

CWKeyer::~CWKeyer() {
    keyerTimer_->stop();
}

void CWKeyer::setIambic(bool value) {
    iambic_ = value;
    qDebug() << "Iambic mode:" << (iambic_ ? "Enabled" : "Disabled");
}

void CWKeyer::setKeyerSpeed(int wpm) {
    speedWpm_ = wpm;
    updateTiming();
    qDebug() << "Keyer speed set to" << wpm << "WPM";
}

void CWKeyer::setBreakIn(bool enabled) {
    breakInEnabled_ = enabled;
    qDebug() << "Break-in:" << (breakInEnabled_ ? "Enabled" : "Disabled");
}

void CWKeyer::setBreakInDelay(int delay) {
    breakInDelay_ = delay;
    qDebug() << "Break-in delay set to" << delay << "ms";
}

void CWKeyer::key(bool state) {
    keyState_ = state;
    sendCW(state);
    if (state) {
        keyerTimer_->start(breakInDelay_);
    } else {
        keyerTimer_->stop();
    }
}

void CWKeyer::onKeyerTimer() {
    if (breakInEnabled_ && keyState_) {
        sendCW(false); // Simulate break-in
        keyerTimer_->stop();
    }
}

void CWKeyer::updateTiming() {
    // Update timing based on speed (WPM)
    // 1 WPM = 50 dots per minute, dot duration = 1200 / WPM ms
    int dotDuration = 1200 / speedWpm_;
    qDebug() << "Dot duration:" << dotDuration << "ms";
    // Placeholder: Update radio timing via Console
    // console_->setCWToneDuration(dotDuration);
}

void CWKeyer::sendCW(bool state) {
    // Placeholder: Send CW signal to radio via Console
    qDebug() << "CW Key:" << (state ? "Down" : "Up");
    // console_->setCWTone(state);
}
