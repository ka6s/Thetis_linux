#include <WaveControl.h>
#include <Console.h>
#include <WaveOptions.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

WaveControl::WaveControl(Console* console, QWidget* parent)
    : QDialog(parent),
      console_(console),
      waveOptionsForm_(nullptr),
      wave_folder_(QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/Thetis"),
      openFileDialog_(new QFileDialog(this)),
      checkBoxPlay_(new QCheckBox("Play")),
      checkBoxRecord_(new QCheckBox("Record")),
      checkBoxPause_(new QCheckBox("Pause")),
      btnStop_(new QPushButton("Stop")),
      btnPrevious_(new QPushButton("Prev")),
      btnNext_(new QPushButton("Next")),
      lstPlaylist_(new QListWidget),
      btnAdd_(new QPushButton("Add...")),
      btnRemove_(new QPushButton("Remove")),
      checkBoxRandom_(new QCheckBox("Random")),
      checkBoxLoop_(new QCheckBox("Loop")),
      txtCurrentFile_(new QLineEdit),
      chkQuickRec_(new QCheckBox("Quick Rec")),
      chkQuickPlay_(new QCheckBox("Quick Play")),
      udPreamp_(new QSpinBox),
      tbPreamp_(new QSlider(Qt::Horizontal)),
      currently_playing_(-1) {
    setWindowTitle("Wave File Controls");
    QDir().mkpath(wave_folder_);
    openFileDialog_->setDirectory(wave_folder_);
    openFileDialog_->setNameFilter("WAV files (*.wav);;All files (*.*)");
    openFileDialog_->setFileMode(QFileDialog::ExistingFiles);
    waveOptionsForm_ = new WaveOptions(this);
    setupUI();
    updatePlaylist();
    setFocusProxy(btnAdd_);
    qDebug() << "WaveControl initialized";
}

WaveControl::~WaveControl() {
    delete waveOptionsForm_;
    qDebug() << "WaveControl destructed";
}

void WaveControl::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Menu
    QMenuBar* menuBar = new QMenuBar;
    QMenu* optionsMenu = menuBar->addMenu("Options");
    QAction* optionsAction = optionsMenu->addAction("Options");
    connect(optionsAction, &QAction::triggered, this, &WaveControl::onOptionsClicked);
    mainLayout->addWidget(menuBar);

    // Playback Group
    QGroupBox* grpPlayback = new QGroupBox("Playback");
    QVBoxLayout* playbackLayout = new QVBoxLayout;
    QHBoxLayout* fileLayout = new QHBoxLayout;
    fileLayout->addWidget(new QLabel("Currently Playing:"));
    txtCurrentFile_->setReadOnly(true);
    fileLayout->addWidget(txtCurrentFile_);
    playbackLayout->addLayout(fileLayout);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(btnStop_);
    buttonLayout->addWidget(checkBoxPlay_);
    buttonLayout->addWidget(checkBoxPause_);
    buttonLayout->addWidget(btnPrevious_);
    buttonLayout->addWidget(btnNext_);
    playbackLayout->addLayout(buttonLayout);
    grpPlayback->setLayout(playbackLayout);
    mainLayout->addWidget(grpPlayback);

    // Playlist Group
    QGroupBox* grpPlaylist = new QGroupBox("Playlist");
    QVBoxLayout* playlistLayout = new QVBoxLayout;
    QHBoxLayout* playlistButtonLayout = new QHBoxLayout;
    playlistButtonLayout->addWidget(btnAdd_);
    playlistButtonLayout->addWidget(btnRemove_);
    playlistButtonLayout->addWidget(checkBoxLoop_);
    playlistButtonLayout->addWidget(checkBoxRandom_);
    playlistLayout->addLayout(playlistButtonLayout);
    lstPlaylist_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    playlistLayout->addWidget(lstPlaylist_);
    grpPlaylist->setLayout(playlistLayout);
    mainLayout->addWidget(grpPlaylist);

    // Record Group
    QGroupBox* groupBox2 = new QGroupBox("Record");
    QHBoxLayout* recordLayout = new QHBoxLayout;
    recordLayout->addWidget(checkBoxRecord_);
    groupBox2->setLayout(recordLayout);
    mainLayout->addWidget(groupBox2);

    // TX Gain Group
    QGroupBox* groupBoxTS1 = new QGroupBox("TX Gain (dB)");
    QVBoxLayout* gainLayout = new QVBoxLayout;
    udPreamp_->setRange(-70, 70);
    udPreamp_->setValue(0);
    gainLayout->addWidget(udPreamp_);
    tbPreamp_->setRange(-70, 70);
    tbPreamp_->setValue(0);
    gainLayout->addWidget(tbPreamp_);
    groupBoxTS1->setLayout(gainLayout);
    mainLayout->addWidget(groupBoxTS1);

    // Quick Record/Play
    QHBoxLayout* quickLayout = new QHBoxLayout;
    quickLayout->addWidget(chkQuickRec_);
    quickLayout->addWidget(chkQuickPlay_);
    mainLayout->addLayout(quickLayout);

    // Note
    QLabel* labelTS1 = new QLabel("NOTE: In Receive, playback goes to the input of the receiver. In Transmit, playback goes to the input of the transmitter.");
    labelTS1->setWordWrap(true);
    mainLayout->addWidget(labelTS1);

    // Connections
    connect(btnAdd_, &QPushButton::clicked, this, &WaveControl::onAddClicked);
    connect(btnRemove_, &QPushButton::clicked, this, &WaveControl::onRemoveClicked);
    connect(checkBoxPlay_, &QCheckBox::toggled, this, &WaveControl::onPlayToggled);
    connect(checkBoxPause_, &QCheckBox::toggled, this, &WaveControl::onPauseToggled);
    connect(btnStop_, &QPushButton::clicked, this, &WaveControl::onStopClicked);
    connect(btnPrevious_, &QPushButton::clicked, this, &WaveControl::onPreviousClicked);
    connect(btnNext_, &QPushButton::clicked, this, &WaveControl::onNextClicked);
    connect(checkBoxRecord_, &QCheckBox::toggled, this, &WaveControl::onRecordToggled);
    connect(chkQuickRec_, &QCheckBox::toggled, this, &WaveControl::onQuickRecToggled);
    connect(chkQuickPlay_, &QCheckBox::toggled, this, &WaveControl::onQuickPlayToggled);
    connect(udPreamp_, QOverload<int>::of(&QSpinBox::valueChanged), this, &WaveControl::onPreampValueChanged);
    connect(tbPreamp_, &QSlider::valueChanged, this, &WaveControl::onPreampSliderMoved);
    connect(lstPlaylist_, &QListWidget::doubleClicked, this, &WaveControl::onPlaylistDoubleClicked);
    connect(lstPlaylist_, &QListWidget::itemSelectionChanged, this, &WaveControl::onPlaylistSelectionChanged);

    checkBoxPlay_->setEnabled(false);
    checkBoxPause_->setEnabled(false);
    btnStop_->setEnabled(false);
    btnPrevious_->setEnabled(false);
    btnNext_->setEnabled(false);
    btnRemove_->setEnabled(false);
    checkBoxLoop_->setEnabled(false);
    checkBoxRandom_->setEnabled(false);
    chkQuickPlay_->setEnabled(false);
}

void WaveControl::updatePlaylist() {
    lstPlaylist_->clear();
    for (const QString& s : file_list_) {
        lstPlaylist_->addItem(QFileInfo(s).baseName());
    }
    if (!file_list_.isEmpty() && lstPlaylist_->currentRow() < 0) {
        lstPlaylist_->setCurrentRow(0);
    }
    bool hasItems = !file_list_.isEmpty();
    checkBoxPlay_->setEnabled(hasItems);
    btnRemove_->setEnabled(hasItems);
    checkBoxLoop_->setEnabled(hasItems);
    checkBoxRandom_->setEnabled(file_list_.size() > 1);
}

bool WaveControl::openWaveFile(const QString& filename, int id) {
    // Simplified: Delegate to Audio class (implemented later)
    if (!QFile::exists(filename)) {
        QMessageBox::critical(this, "Bad Filename", "Filename doesn't exist: " + filename);
        if (currently_playing_ >= 0) {
            file_list_.removeAt(currently_playing_);
        }
        return false;
    }
    // Placeholder for actual WAV file validation and playback setup
    qDebug() << "Opening WAV file:" << filename << "ID:" << id;
    return true; // Implement in Audio class
}

bool WaveControl::checkSampleRate(int rate) {
    return rate == 6000 || rate == 12000 || rate == 24000 || rate == 48000 ||
           rate == 96000 || rate == 192000 || rate == 384000;
}

void WaveControl::onAddClicked() {
    if (openFileDialog_->exec() == QDialog::Accepted) {
        for (const QString& file : openFileDialog_->selectedFiles()) {
            if (QFileInfo(file).suffix().toLower() == "wav" && !file_list_.contains(file)) {
                file_list_.append(file);
            }
        }
        updatePlaylist();
    }
}

void WaveControl::onRemoveClicked() {
    if (lstPlaylist_->count() == 0 || lstPlaylist_->selectedItems().isEmpty()) {
        return;
    }
    QList<int> indices;
    for (QListWidgetItem* item : lstPlaylist_->selectedItems()) {
        int index = lstPlaylist_->row(item);
        if (index == currently_playing_ && checkBoxPlay_->isChecked()) {
            if (QMessageBox::question(this, "Stop and Remove?",
                                      lstPlaylist_->item(index)->text() + " is currently playing.\nStop playing and remove from Playlist?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                indices.append(index);
                checkBoxPlay_->setChecked(false);
            }
        } else {
            indices.append(index);
        }
    }
    std::sort(indices.begin(), indices.end(), std::greater<int>());
    for (int i : indices) {
        file_list_.removeAt(i);
    }
    updatePlaylist();
}

void WaveControl::onPlayToggled(bool checked) {
    if (checked) {
        if (currently_playing_ < 0 || currently_playing_ >= file_list_.size()) {
            checkBoxPlay_->setChecked(false);
            return;
        }
        QString filename = file_list_[currently_playing_];
        if (!openWaveFile(filename, 0)) {
            checkBoxPlay_->setChecked(false);
            currently_playing_ = -1;
            updatePlaylist();
            return;
        }
        txtCurrentFile_->setText(lstPlaylist_->item(currently_playing_)->text());
        checkBoxPlay_->setStyleSheet("background-color: yellow");
        checkBoxPause_->setEnabled(true);
    } else {
        // Stop playback (implement in Audio)
        if (checkBoxPause_->isChecked()) {
            checkBoxPause_->setChecked(false);
        }
        checkBoxPause_->setEnabled(false);
        txtCurrentFile_->clear();
        checkBoxPlay_->setStyleSheet("");
    }
    console_->setWavePlayback(checked);
}

void WaveControl::onPauseToggled(bool checked) {
    console_->setWavePlayback(checkBoxPlay_->isChecked() && !checked);
    checkBoxPause_->setStyleSheet(checked ? "background-color: yellow" : "");
}

void WaveControl::onStopClicked() {
    checkBoxPlay_->setChecked(false);
}

void WaveControl::onPreviousClicked() {
    if (checkBoxPlay_->isChecked()) {
        checkBoxPlay_->setChecked(false);
        currently_playing_--;
        checkBoxPlay_->setChecked(true);
    } else {
        lstPlaylist_->setCurrentRow(lstPlaylist_->currentRow() - 1);
    }
}

void WaveControl::onNextClicked() {
    if (checkBoxPlay_->isChecked()) {
        checkBoxPlay_->setChecked(false);
        if (currently_playing_ == lstPlaylist_->count() - 1) {
            currently_playing_ = 0;
        } else {
            currently_playing_++;
        }
        checkBoxPlay_->setChecked(true);
    } else {
        int next = lstPlaylist_->currentRow() + 1;
        if (next == lstPlaylist_->count()) {
            next = 0;
        }
        lstPlaylist_->setCurrentRow(next);
    }
}

void WaveControl::onRecordToggled(bool checked) {
    if (checked) {
        checkBoxRecord_->setStyleSheet("background-color: yellow");
        QString filename = wave_folder_ + "/" + console_->getRX1DSPMode() + " " +
                           QString::number(console_->getVFOAFreq(), 'f', 6) + "MHz [" +
                           QString::number(console_->getSampleRate() / 1000) + "k] " +
                           QDateTime::currentDateTime().toString("yyyy-MM-dd hh mm ss") + ".wav";
        filename.replace("/", "-");
        // Start recording (implement in Audio)
        qDebug() << "Recording to:" << filename;
    } else {
        // Stop recording
        checkBoxRecord_->setStyleSheet("");
    }
    console_->setWaveRecord(checked);
}

void WaveControl::onQuickRecToggled(bool checked) {
    if (checked) {
        chkQuickRec_->setStyleSheet("background-color: yellow");
        chkQuickPlay_->setEnabled(true);
        QString filename = console_->getAppDataPath() + "SDRQuickAudio.wav";
        // Start recording (implement in Audio)
        qDebug() << "Quick recording to:" << filename;
    } else {
        // Stop recording
        chkQuickRec_->setStyleSheet("");
    }
    console_->setWaveRecord(checked);
}

void WaveControl::onQuickPlayToggled(bool checked) {
    if (checked) {
        QString filename = console_->getAppDataPath() + "SDRQuickAudio.wav";
        if (!openWaveFile(filename, 0)) {
            chkQuickPlay_->setChecked(false);
            return;
        }
        chkQuickPlay_->setStyleSheet("background-color: yellow");
    } else {
        // Stop playback
        chkQuickPlay_->setStyleSheet("");
    }
    console_->setWavePlayback(checked);
}

void WaveControl::onPreampValueChanged(int value) {
    tbPreamp_->setValue(value);
    double gain = std::pow(10.0, value / 20.0);
    console_->setWavePreamp(gain);
}

void WaveControl::onPreampSliderMoved(int value) {
    udPreamp_->setValue(value);
}

void WaveControl::onPlaylistDoubleClicked(const QModelIndex& index) {
    if (checkBoxPlay_->isChecked()) {
        currently_playing_ = index.row();
        checkBoxPlay_->setChecked(false);
        checkBoxPlay_->setChecked(true);
    } else {
        checkBoxPlay_->setChecked(true);
    }
}

void WaveControl::onPlaylistSelectionChanged() {
    if (lstPlaylist_->currentRow() < 0) {
        btnPrevious_->setEnabled(false);
        btnNext_->setEnabled(false);
        return;
    }
    if (!checkBoxPlay_->isChecked()) {
        currently_playing_ = lstPlaylist_->currentRow();
        btnPrevious_->setEnabled(currently_playing_ > 0);
        btnNext_->setEnabled(checkBoxLoop_->isChecked() || currently_playing_ < lstPlaylist_->count() - 1);
    }
}

void WaveControl::onOptionsClicked() {
    if (!waveOptionsForm_->isVisible()) {
        waveOptionsForm_->show();
    }
    waveOptionsForm_->raise();
    waveOptionsForm_->activateWindow();
}

void WaveControl::next() {
    if (checkBoxPlay_->isChecked()) {
        if (btnNext_->isEnabled()) {
            onNextClicked();
        } else if (checkBoxLoop_->isChecked() && lstPlaylist_->count() == 1) {
            onPlayToggled(true);
        } else {
            checkBoxPlay_->setChecked(false);
        }
    }
    if (chkQuickPlay_->isChecked()) {
        chkQuickPlay_->setChecked(false);
    }
}

bool WaveControl::quickRec() const {
    return chkQuickRec_->isChecked();
}

void WaveControl::setQuickRec(bool value) {
    chkQuickRec_->setChecked(value);
}

bool WaveControl::quickPlay() const {
    return chkQuickPlay_->isChecked();
}

void WaveControl::setQuickPlay(bool value) {
    chkQuickPlay_->setChecked(value);
}
