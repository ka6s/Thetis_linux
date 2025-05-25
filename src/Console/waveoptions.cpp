#include <WaveOptions.h>
#include <QDebug>

WaveOptions::WaveOptions(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Wave Options");
    setupUI();
    qDebug() << "WaveOptions initialized";
}

WaveOptions::~WaveOptions() {
    qDebug() << "WaveOptions destructed";
}

void WaveOptions::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Sample Rate
    QLabel* sampleRateLabel = new QLabel("Sample Rate (Hz):", this);
    sampleRateCombo_ = new QComboBox(this);
    sampleRateCombo_->addItems({"6000", "12000", "24000", "48000", "96000", "192000", "384000"});
    sampleRateCombo_->setCurrentText("48000");
    mainLayout->addWidget(sampleRateLabel);
    mainLayout->addWidget(sampleRateCombo_);

    // Channels
    QLabel* channelsLabel = new QLabel("Channels:", this);
    channelsCombo_ = new QComboBox(this);
    channelsCombo_->addItems({"Mono (1)", "Stereo (2)"});
    channelsCombo_->setCurrentText("Stereo (2)");
    mainLayout->addWidget(channelsLabel);
    mainLayout->addWidget(channelsCombo_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    applyButton_ = new QPushButton("Apply", this);
    cancelButton_ = new QPushButton("Cancel", this);
    buttonLayout->addWidget(applyButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(applyButton_, &QPushButton::clicked, this, &WaveOptions::applySettings);
    connect(cancelButton_, &QPushButton::clicked, this, &WaveOptions::cancelSettings);

    setLayout(mainLayout);
}

void WaveOptions::applySettings() {
    qDebug() << "WaveOptions: Sample Rate =" << sampleRateCombo_->currentText()
             << "Channels =" << channelsCombo_->currentText();
    // TODO: Integrate with Audio class for WAV settings
    accept();
}

void WaveOptions::cancelSettings() {
    reject();
}
