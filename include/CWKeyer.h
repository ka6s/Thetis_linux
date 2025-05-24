#ifndef CWKEYER_H
#define CWKEYER_H

#include <QObject>
#include <QTimer>
#include <QString>

class Console; // Forward declaration

class CWKeyer : public QObject {
    Q_OBJECT

public:
    explicit CWKeyer(Console* console, QObject* parent = nullptr);
    ~CWKeyer();

    void setIambic(bool value);
    void setKeyerSpeed(int wpm);
    void setBreakIn(bool enabled);
    void setBreakInDelay(int delay);
    void key(bool state); // Simulates key down/up

private slots:
    void onKeyerTimer();

private:
    void updateTiming();
    void sendCW(bool state);

    Console* console_;
    bool iambic_;
    int speedWpm_; // Words per minute
    bool breakInEnabled_;
    int breakInDelay_; // Milliseconds
    QTimer* keyerTimer_;
    bool keyState_;
};

#endif // CWKEYER_H
