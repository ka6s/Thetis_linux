#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QStringList>

class Console;

class Filter : public QObject {
    Q_OBJECT

public:
    explicit Filter(Console* console, QObject* parent = nullptr);
    ~Filter();

    QStringList getFilterTypes() const;
    void setFilterType(const QString& type);
    void setFilterBandwidth(int bandwidth);

private:
    Console* console_;
    QString currentFilterType_;
    QStringList filterTypes_;
};

#endif // FILTER_H
