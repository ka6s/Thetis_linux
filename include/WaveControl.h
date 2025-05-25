#ifndef WAVECONTROL_H
#define WAVECONTROL_H

#include <QWidget>
#include <QObject>

class Console;
class WaveOptions;
class QPushButton;
class QLineEdit;

class WaveControl : public QWidget {
    Q_OBJECT

public:
    explicit WaveControl(Console* console, QWidget* parent = nullptr);
    ~WaveControl();

    void showOptions();

private slots:
    void onPlayClicked();
    void onRecordClicked();
    void onStopClicked();
    void onOptionsClicked();

private:
    Console* console_; // Added
    WaveOptions* waveOptionsForm_;
    QPushButton* playButton_;
    QPushButton* recordButton_;
    QPushButton* stopButton_;
    QPushButton* optionsButton_;
    QLineEdit* filePathEdit_;
    int playbackId_;
};

#endif // WAVECONTROL_H
