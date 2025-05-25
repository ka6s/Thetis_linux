#ifndef SETUP_H
#define SETUP_H

#include <QDialog>
#include <QTabWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QDoubleSpinBox>

class Console;
class Filter;
class VFO;

class Setup : public QDialog {
    Q_OBJECT

public:
    explicit Setup(Console* console, Filter* filter, VFO* vfo, QWidget* parent = nullptr);
    ~Setup();

private slots:
    void applySettings();
    void cancelSettings();

private:
    void setupUI();

    Console* console_;
    Filter* filter_;
    VFO* vfo_;
    QTabWidget* tabWidget_;
    QComboBox* sampleRateCombo_;
    QComboBox* modeCombo_;
    QComboBox* filterTypeCombo_;
    QSpinBox* filterBandwidthSpin_;
    QDoubleSpinBox* frequencySpin_;
    QComboBox* vfoModeCombo_;
    QSpinBox* vfoStepSpin_;
    QPushButton* applyButton_;
    QPushButton* cancelButton_;
};

#endif // SETUP_H
