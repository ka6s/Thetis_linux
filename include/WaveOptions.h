#ifndef WAVEOPTIONS_H
#define WAVEOPTIONS_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class WaveOptions : public QDialog {
    Q_OBJECT

public:
    explicit WaveOptions(QWidget* parent = nullptr);
    ~WaveOptions();

private slots:
    void applySettings();
    void cancelSettings();

private:
    void setupUI();

    QComboBox* sampleRateCombo_;
    QComboBox* channelsCombo_;
    QPushButton* applyButton_;
    QPushButton* cancelButton_;
};

#endif // WAVEOPTIONS_H
