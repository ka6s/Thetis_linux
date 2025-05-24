#include <Setup.h>
#include <Console.h>
#include <Filter.h>
#include <VFO.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

Setup::Setup(Console* console, QWidget* parent)
    : QDialog(parent),
      console_(console),
      filter_(new Filter(console, this)),
      vfo_(new VFO(console, this)) {
    setWindowTitle("Thetis Setup");
    setupUI();
}

Setup::~Setup() {
}

void Setup::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    tabWidget_ = new QTabWidget(this);

    // General Tab
    QWidget* generalTab = new QWidget();
    QVBoxLayout* generalLayout = new QVBoxLayout(generalTab);

    // Sample Rate
    QLabel* sampleRateLabel = new QLabel("Sample Rate (Hz):", generalTab);
    sampleRateCombo_ = new QComboBox(generalTab);
    sampleRateCombo_->addItems({"48000", "96000", "192000", "384000"});
    sampleRateCombo_->setCurrentText("48000");
    generalLayout->addWidget(sampleRateLabel);
    generalLayout->addWidget(sampleRateCombo_);

    // Mode
    QLabel* modeLabel = new QLabel("Mode:", generalTab);
    modeCombo_ = new QComboBox(generalTab);
    modeCombo_->addItems({"LSB", "USB", "CW", "AM"});
    modeCombo_->setCurrentText("USB");
    generalLayout->addWidget(modeLabel);
    generalLayout->addWidget(modeCombo_);

    // Frequency
    QLabel* frequencyLabel = new QLabel("Frequency (MHz):", generalTab);
    frequencySpin_ = new QDoubleSpinBox(generalTab);
    frequencySpin_->setRange(0.1, 30.0);
    frequencySpin_->setValue(7.0);
    frequencySpin_->setDecimals(6);
    generalLayout->addWidget(frequencyLabel);
    generalLayout->addWidget(frequencySpin_);

    // VFO Mode
    QLabel* vfoModeLabel = new QLabel("VFO Mode:", generalTab);
    vfoModeCombo_ = new QComboBox(generalTab);
    vfoModeCombo_->addItems(vfo_->getVFOModes());
    vfoModeCombo_->setCurrentText("VFO A");
    generalLayout->addWidget(vfoModeLabel);
    generalLayout->addWidget(vfoModeCombo_);

    // VFO Step Size
    QLabel* vfoStepLabel = new QLabel("VFO Step Size (Hz):", generalTab);
    vfoStepSpin_ = new QSpinBox(generalTab);
    vfoStepSpin_->setRange(10, 10000);
    vfoStepSpin_->setValue(100);
    generalLayout->addWidget(vfoStepLabel);
    generalLayout->addWidget(vfoStepSpin_);

    generalLayout->addStretch();
    tabWidget_->addTab(generalTab, "General");

    // DSP Tab
    QWidget* dspTab = new QWidget();
    QVBoxLayout* dspLayout = new QVBoxLayout(dspTab);

    // Filter Type
    QLabel* filterTypeLabel = new QLabel("Filter Type:", dspTab);
    filterTypeCombo_ = new QComboBox(dspTab);
    filterTypeCombo_->addItems(filter_->getFilterTypes());
    filterTypeCombo_->setCurrentText("Bandpass");
    dspLayout->addWidget(filterTypeLabel);
    dspLayout->addWidget(filterTypeCombo_);

    // Filter Bandwidth
    QLabel* filterLabel = new QLabel("Filter Bandwidth (Hz):", dspTab);
    filterBandwidthSpin_ = new QSpinBox(dspTab);
    filterBandwidthSpin_->setRange(100, 10000);
    filterBandwidthSpin_->setValue(3000);
    dspLayout->addWidget(filterLabel);
    dspLayout->addWidget(filterBandwidthSpin_);

    dspLayout->addStretch();
    tabWidget_->addTab(dspTab, "DSP");

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    applyButton_ = new QPushButton("Apply", this);
    cancelButton_ = new QPushButton("Cancel", this);
    buttonLayout->addWidget(applyButton_);
    buttonLayout->addWidget(cancelButton_);

    mainLayout->addWidget(tabWidget_);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(applyButton_, &QPushButton::clicked, this, &Setup::applySettings);
    connect(cancelButton_, &QPushButton::clicked, this, &Setup::cancelSettings);

    setLayout(mainLayout);
}

void Setup::applySettings() {
    // Apply settings to Console
    int sampleRate = sampleRateCombo_->currentText().toInt();
    console_->setSampleRate(sampleRate);

    QString mode;
    if (modeCombo_->currentText() == "LSB") mode = "1";
    else if (modeCombo_->currentText() == "USB") mode = "2";
    else if (modeCombo_->currentText() == "CW") mode = "6";
    else if (modeCombo_->currentText() == "AM") mode = "3";
    console_->setMode(mode);

    qint64 frequency = static_cast<qint64>(frequencySpin_->value() * 1e6);
    vfo_->setFrequency(frequency);

    QString vfoMode = vfoModeCombo_->currentText();
    vfo_->setVFOMode(vfoMode);

    int vfoStep = vfoStepSpin_->value();
    vfo_->setStepSize(vfoStep);

    QString filterType = filterTypeCombo_->currentText();
    filter_->setFilterType(filterType);

    int filterBandwidth = filterBandwidthSpin_->value();
    filter_->setFilterBandwidth(filterBandwidth);

    qDebug() << "Applied settings: Sample Rate =" << sampleRate
             << "Mode =" << mode
             << "Frequency =" << frequency
             << "VFO Mode =" << vfoMode
             << "VFO Step =" << vfoStep
             << "Filter Type =" << filterType
             << "Filter Bandwidth =" << filterBandwidth;

    accept();
}

void Setup::cancelSettings() {
    reject();
}
