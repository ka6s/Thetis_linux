#ifndef INCLUDED_DISPLAY_H
#define INCLUDED_DISPLAY_H

#include <QWidget>
#include <vector>

class Console;

class SpectrumWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpectrumWidget(QWidget* parent = nullptr);
    void setData(const std::vector<float>& data, double centerFrequency, int bandwidth);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void frequencySelected(double freq);

private:
    std::vector<float> spectrumData_;
    double centerFrequency_;
    int bandwidth_;
};

class Display : public QWidget {
    Q_OBJECT

public:
    explicit Display(Console* console, QWidget* parent = nullptr);
    ~Display();

    void setCenterFrequency(double freq);
    void setBandwidth(int bw);
    void updateSpectrum(const float* data, int size);

signals:
    void frequencyChanged(double freq);

private:
    Console* palette_;
    SpectrumWidget* spectrumWidget_;
    std::vector<float> spectrumData_; // Added
    double centerFrequency_;
    int bandwidth_;
};

#endif // INCLUDED_DISPLAY_H
