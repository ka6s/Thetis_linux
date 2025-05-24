#ifndef SETUP_H
#define SETUP_H

#include <QDialog>
#include <QTabWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

class Console;
class Filter;

class Setup : public QDialog {
    Q_OBJECT

public:
    explicit Setup(Console* console, QWidget* parent = nullptr);
    ~Setup();

private slots:
    void applySettings();
    void cancelSettings();

private:
    void setupUI();

    Console* console_;
    Filter* filter_;
    QTabWidget* tabWidget_;
    QComboBox* sampleRateCombo_;
    QComboBox* modeCombo_;
    QComboBox* filterTypeCombo_;
    QSpinBox* filterBandwidthSpin_;
    QPushButton* applyButton_;
    QPushButton* cancelButton_;
};

#endif // SETUP_H
