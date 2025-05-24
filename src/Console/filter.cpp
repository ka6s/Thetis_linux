#include <Filter.h>
#include <Console.h>
#include <wdsp.h>
#include <QDebug>

Filter::Filter(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      currentFilterType_("Bandpass"),
      filterTypes_({"Bandpass", "LowPass", "HighPass", "Notch"}) {
    qDebug() << "Filter initialized with type:" << currentFilterType_;
}

Filter::~Filter() {
}

QStringList Filter::getFilterTypes() const {
    return filterTypes_;
}

void Filter::setFilterType(const QString& type) {
    if (filterTypes_.contains(type)) {
        currentFilterType_ = type;
        qDebug() << "Filter type set to:" << type;
    } else {
        qDebug() << "Invalid filter type:" << type;
    }
}

void Filter::setFilterBandwidth(int bandwidth) {
    qDebug() << "Setting filter bandwidth:" << bandwidth << "Hz";
    console_->setFilterBandwidth(bandwidth);
}
