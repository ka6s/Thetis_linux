#ifndef WAVEOPTIONS_H
#define WAVEOPTIONS_H

#include <QDialog>
#include <QObject>

class Console;
class QComboBox;
class QLineEdit;
class QPushButton;

class WaveOptions : public QDialog {
    Q_OBJECT

public:
    explicit WaveOptions(Console* console, QWidget* parent = nullptr);
    ~WaveOptions();

    void setSampleRate(int rate);
    int getSampleRate() const;
    void setChannels(int channels);
    int getChannels() const;
    void setRecordPath(const QString& path);
    QString getRecordPath() const;

private slots:
    void onSampleRateChanged(const QString& rate);
    void onChannelsChanged(const QString& channels);
    void onBrowseClicked();
    void onAccept();
    void onReject();

private:
    Console* console_;
    QComboBox* sampleRateCombo_;
    QComboBox* channelsCombo_;
    QLineEdit* recordPathEdit_;
    QPushButton* browseButton_;
    QPushButton* okButton_;
    QPushButton* cancelButton_;
    int sampleRate_;
    int channels_;
    QString recordPath_;
};

#endif // WAVEOPTIONS_H
