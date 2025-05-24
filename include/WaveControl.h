#ifndef WAVECONTROL_H
#define WAVECONTROL_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QMenuBar>
#include <QFileDialog>

class Console;
class WaveOptions;

class WaveControl : public QDialog {
    Q_OBJECT

public:
    explicit WaveControl(Console* console, QWidget* parent = nullptr);
    ~WaveControl();

    bool quickRec() const;
    void setQuickRec(bool value);
    bool quickPlay() const;
    void setQuickPlay(bool value);

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onPlayToggled(bool checked);
    void onPauseToggled(bool checked);
    void onStopClicked();
    void onPreviousClicked();
    void onNextClicked();
    void onRecordToggled(bool checked);
    void onQuickRecToggled(bool checked);
    void onQuickPlayToggled(bool checked);
    void onPreampValueChanged(int value);
    void onPreampSliderMoved(int value);
    void onPlaylistDoubleClicked(const QModelIndex& index);
    void onPlaylistSelectionChanged();
    void onOptionsClicked();

private:
    void setupUI();
    void updatePlaylist();
    bool openWaveFile(const QString& filename, int id);
    bool checkSampleRate(int rate);
    void next();

    Console* console_;
    WaveOptions* waveOptionsForm_;
    QStringList file_list_;
    QString wave_folder_;
    QFileDialog* openFileDialog_;
    QCheckBox* checkBoxPlay_;
    QCheckBox* checkBoxRecord_;
    QCheckBox* checkBoxPause_;
    QPushButton* btnStop_;
    QPushButton* btnPrevious_;
    QPushButton* btnNext_;
    QListWidget* lstPlaylist_;
    QPushButton* btnAdd_;
    QPushButton* btnRemove_;
    QCheckBox* checkBoxRandom_;
    QCheckBox* checkBoxLoop_;
    QLineEdit* txtCurrentFile_;
    QCheckBox* chkQuickRec_;
    QCheckBox* chkQuickPlay_;
    QSpinBox* udPreamp_;
    QSlider* tbPreamp_;
    int currently_playing_;
};

#endif // WAVECONTROL_H
