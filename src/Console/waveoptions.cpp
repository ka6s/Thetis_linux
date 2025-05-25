#include <WaveOptions.h>
#include <Console.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>

WaveOptions::WaveOptions(Console* console, QWidget* parent)
    : QDialog(parent),
      console_(console),
      sampleRateCombo_(new QComboBox(this)),
      channelsCombo_(new QComboBox(this)),
      recordPathEdit_(new QLineEdit(this)),
      browseButton_(new QPushButton("Browse", this)),
      okButton_(new QPushButton("OK", this)),
      cancelButton_(new QPushButton("Cancel", this)),
      sampleRate_(48000),
      channels_(2),
      recordPath_(QDir::homePath() + "/Thetis") {
    setWindowTitle("Wave File Options");

    // Sample rate options (from WaveOptions.cs)
    QStringList sampleRates = {"8000", "11025", "12000", "16000", "22050", "24000",
                              "44100", "48000", "96000", "192000"};
    sampleRateCombo_->addItems(sampleRates);
    sampleRateCombo_->setCurrentText("48000");

    // Channel options
    QStringList channelsList = {"Mono", "Stereo"};
    channelsCombo_->addItems(channelsList);
    channelsCombo_->setCurrentText("Stereo");

    // Record path
    recordPathEdit_->setText(recordPath_);

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* sampleRateLayout = new QHBoxLayout();
    QHBoxLayout* channelsLayout = new QHBoxLayout();
    QHBoxLayout* pathLayout = new QHBoxLayout();
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    sampleRateLayout->addWidget(new QLabel("Sample Rate (Hz):", this));
    sampleRateLayout->addWidget(sampleRateCombo_);
    channelsLayout->addWidget(new QLabel("Channels:", this));
    channelsLayout->addWidget(channelsCombo_);
    pathLayout->addWidget(new QLabel("Record Path:", this));
    pathLayout->addWidget(recordPathEdit_);
    pathLayout->addWidget(browseButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);

    mainLayout->addLayout(sampleRateLayout);
    mainLayout->addLayout(channelsLayout);
    mainLayout->addLayout(pathLayout);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(sampleRateCombo_, &QComboBox::currentTextChanged, this, &WaveOptions::onSampleRateChanged);
    connect(channelsCombo_, &QComboBox::currentTextChanged, this, &WaveOptions::onChannelsChanged);
    connect(browseButton_, &QPushButton::clicked, this, &WaveOptions::onBrowseClicked);
    connect(okButton_, &QPushButton::clicked, this, &WaveOptions::onAccept);
    connect(cancelButton_, &QPushButton::clicked, this, &WaveOptions::onReject);

    setLayout(mainLayout);
    qDebug() << "WaveOptions dialog initialized";
}

WaveOptions::~WaveOptions() {
    qDebug() << "WaveOptions dialog destroyed";
}

void WaveOptions::setSampleRate(int rate) {
    sampleRate_ = rate;
    sampleRateCombo_->setCurrentText(QString::number(rate));
}

int WaveOptions::getSampleRate() const {
    return sampleRate_;
}

void WaveOptions::setChannels(int channels) {
    channels_ = channels;
    channelsCombo_->setCurrentText(channels == 1 ? "Mono" : "Stereo");
}

int WaveOptions::getChannels() const {
    return channels_;
}

void WaveOptions::setRecordPath(const QString& path) {
    recordPath_ = path;
    recordPathEdit_->setText(path);
}

QString WaveOptions::getRecordPath() const {
    return recordPath_;
}

void WaveOptions::onSampleRateChanged(const QString& rate) {
    bool ok;
    sampleRate_ = rate.toInt(&ok);
    if (!ok) {
        qDebug() << "WaveOptions: Invalid sample rate" << rate;
        sampleRate_ = 48000;
    }
    qDebug() << "WaveOptions: Sample rate set to" << sampleRate_;
}

void WaveOptions::onChannelsChanged(const QString& text) {
    channels_ = (text == "Mono" ? 1 : 2);
    qDebug() << "WaveOptions: Channels set to" << text;
}

void WaveOptions::onBrowseClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Recording Directory",
                                                    recordPath_, QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        setRecordPath(dir);
        qDebug() << "WaveOptions: Record path set to" << dir;
    }
}

void WaveOptions::onAccept() {
    // Placeholder: Save settings via console
    if (console_) {
        qDebug() << "WaveOptions: Saving settings - SampleRate:" << sampleRate_
                 << "Channels:" << channels_ << "RecordPath:" << recordPath_;
        // TODO: Implement console_->saveSettings(sampleRate_, channels_, recordPath_);
    }
    accept();
}

void WaveOptions::onReject() {
    reject();
}
