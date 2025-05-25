#include <WaveControl.h>
#include <Console.h>
#include <WaveOptions.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDebug>

WaveControl::WaveControl(Console* console, QWidget* parent)
    : QWidget(parent),
      console_(console),
      waveOptionsForm_(nullptr),
      playButton_(new QPushButton("Play", this)),
      recordButton_(new QPushButton("Record", this)),
      stopButton_(new QPushButton("Stop", this)),
      optionsButton_(new QPushButton("Options", this)),
      filePathEdit_(new QLineEdit(this)),
      playbackId_(0) {
    setWindowTitle("Wave Control");

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(playButton_);
    buttonLayout->addWidget(recordButton_);
    buttonLayout->addWidget(stopButton_);
    buttonLayout->addWidget(optionsButton_);
    mainLayout->addWidget(filePathEdit_);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(playButton_, &QPushButton::clicked, this, &WaveControl::onPlayClicked);
    connect(recordButton_, &QPushButton::clicked, this, &WaveControl::onRecordClicked);
    connect(stopButton_, &QPushButton::clicked, this, &WaveControl::onStopClicked);
    connect(optionsButton_, &QPushButton::clicked, this, &WaveControl::onOptionsClicked);

    // Initialize WaveOptions
    waveOptionsForm_ = new WaveOptions(console_, this); // Fixed: Pass console_

    setLayout(mainLayout);
    qDebug() << "WaveControl initialized";
}

WaveControl::~WaveControl() {
    if (waveOptionsForm_) {
        waveOptionsForm_->deleteLater();
    }
    qDebug() << "WaveControl destroyed";
}

void WaveControl::showOptions() {
    if (waveOptionsForm_) {
        waveOptionsForm_->show();
    }
}

void WaveControl::onPlayClicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Select WAV File", "",
                                                    "WAV Files (*.wav)");
    if (!filename.isEmpty()) {
        filePathEdit_->setText(filename);
        if (console_) {
            // Placeholder: Use console_->audio() or similar
            qDebug() << "WaveControl: Playing" << filename << "ID:" << playbackId_;
            // console_->audio()->startPlayback(filename, playbackId_++);
        }
    }
}

void WaveControl::onRecordClicked() {
    QString filename = QFileDialog::getSaveFileName(this, "Save WAV File",
                                                    waveOptionsForm_->getRecordPath(),
                                                    "WAV Files (*.wav)");
    if (!filename.isEmpty()) {
        filePathEdit_->setText(filename);
        if (console_) {
            // Placeholder: Use console_->audio()
            qDebug() << "WaveControl: Recording to" << filename
                     << "Channels:" << waveOptionsForm_->getChannels()
                     << "SampleRate:" << waveOptionsForm_->getSampleRate();
            // console_->audio()->startRecording(filename, waveOptionsForm_->getChannels(),
            //                                   waveOptionsForm_->getSampleRate());
        }
    }
}

void WaveControl::onStopClicked() {
    if (console_) {
        // Placeholder: Use console_->audio()
        qDebug() << "WaveControl: Stopping playback/recording";
        // console_->audio()->stopPlayback(playbackId_ - 1);
        // console_->audio()->stopRecording();
    }
}

void WaveControl::onOptionsClicked() {
    showOptions();
    qDebug() << "WaveControl: Showing options dialog";
}
