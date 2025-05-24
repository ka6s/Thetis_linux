#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class Console;

class Display : public QWidget {
    Q_OBJECT

public:
    explicit Display(Console* console, QWidget* parent = nullptr);
    ~Display();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void updateDisplay();

private:
    Console* console_;
    QTimer* timer_;
    QVector<float> spectrumData_;
};

#endif // DISPLAY_H
