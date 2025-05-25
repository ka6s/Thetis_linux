#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class Console;
class NetworkIO;

class Display : public QWidget {
    Q_OBJECT

public:
    explicit Display(Console* console, NetworkIO* networkIO = nullptr, QWidget* parent = nullptr);
    ~Display();

public slots:
    void updateSpectrumData(const QVector<float>& data);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void updateDisplay();

private:
    Console* console_;
    NetworkIO* networkIO_;
    QTimer* timer_;
    QVector<float> spectrumData_;
    qint64 updateCount_; // Added to track updates
};

#endif // DISPLAY_H
