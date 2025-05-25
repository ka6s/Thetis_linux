#include <NetworkIO.h>
#include <Console.h>
#include <QDebug>
#include <fftw3.h>
#include <cmath>
#include <algorithm>

NetworkIO::NetworkIO(Console* console, QObject* parent)
    : QObject(parent),
      console_(console),
      udpSocket_(new QUdpSocket(this)),
      host_("localhost"),
      port_(50001),
      frequency_(14.0e6),
      gain_(1.0), // Reduced gain
      iqBuffer_(BUFFER_SIZE, 0.0f),
      running_(false) {
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    connect(udpSocket_, &QUdpSocket::readyRead, this, &NetworkIO::processPendingDatagrams);
    connect(udpSocket_, &QUdpSocket::errorOccurred, this, [](QAbstractSocket::SocketError error) {
        qDebug() << "NetworkIO: Socket error:" << error;
    });
    qDebug() << "NetworkIO initialized";
}

NetworkIO::~NetworkIO() {
    stop();
    qDebug() << "NetworkIO destroyed";
}

void NetworkIO::setHost(const QString& host, int port) {
    host_ = host;
    port_ = port;
    qDebug() << "NetworkIO: Host set to" << host << "port" << port;
}

void NetworkIO::setFrequency(double freq) {
    frequency_ = freq;
    qDebug() << "NetworkIO: Frequency set to" << freq << "Hz";
}

void NetworkIO::setGain(double gain) {
    gain_ = gain;
    qDebug() << "NetworkIO: Gain set to" << gain;
}

void NetworkIO::start() {
    if (running_) return;
    if (!udpSocket_->bind(QHostAddress::Any, port_, QUdpSocket::ReuseAddressHint)) {
        emit errorOccurred("Failed to bind UDP socket: " + udpSocket_->errorString());
        qDebug() << "NetworkIO: Bind failed on port" << port_;
        return;
    }
    running_ = true;
    qDebug() << "NetworkIO started on port" << port_;
}

void NetworkIO::stop() {
    if (!running_) return;
    running_ = false;
    udpSocket_->close();
    qDebug() << "NetworkIO stopped";
}

void NetworkIO::processPendingDatagrams() {
    while (running_ && udpSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpSocket_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        qDebug() << "NetworkIO: Received datagram, size:" << datagram.size()
                 << "from" << sender.toString() << ":" << senderPort;
        processIQData(datagram);
    }
}

void NetworkIO::processIQData(const QByteArray& data) {
    int floatSize = sizeof(float);
    if (data.size() < 2 * floatSize) {
        qDebug() << "NetworkIO: Datagram too small, size:" << data.size();
        return;
    }
    int sampleCount = data.size() / (2 * floatSize);
    qDebug() << "NetworkIO: Processing" << sampleCount << "I/Q sample pairs";

    const float* samples = reinterpret_cast<const float*>(data.constData());
    qDebug() << "NetworkIO: Sample values (first 4):"
             << samples[0] << samples[1] << samples[2] << samples[3];

    iqBuffer_.clear();
    for (int i = 0; i < sampleCount * 2; ++i) {
        iqBuffer_.push_back(samples[i]);
    }

    if (iqBuffer_.size() >= FFT_SIZE * 2) {
        computeSpectrum(iqBuffer_.data(), FFT_SIZE);
        iqBuffer_.erase(iqBuffer_.begin(), iqBuffer_.begin() + FFT_SIZE * 2);
        qDebug() << "NetworkIO: Computed spectrum, buffer size now:" << iqBuffer_.size();
    }
}

void NetworkIO::computeSpectrum(const float* iqData, int size) {
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
    fftw_plan plan = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Apply Hann window
    std::vector<float> window(size);
    float sum = 0.0f;
    for (int i = 0; i < size; ++i) {
        window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (size - 1)));
        sum += window[i];
    }
    float scale = 1.0f;

    float maxInputRaw = 0.0f;
    for (int i = 0; i < size * 2; ++i) {
        maxInputRaw = std::max(maxInputRaw, fabsf(iqData[i]));
        if (i < 4) qDebug() << "NetworkIO: iqData[" << i << "]:" << iqData[i];
    }
    qDebug() << "NetworkIO: Raw input max amplitude:" << maxInputRaw;

    float maxInput = 0.0f;
    for (int i = 0; i < size; ++i) {
        in[i][0] = iqData[2 * i] * window[i] * scale * gain_;
        in[i][1] = iqData[2 * i + 1] * window[i] * scale * gain_;
        maxInput = std::max(maxInput, std::max(fabsf(in[i][0]), fabsf(in[i][1])));
    }
    qDebug() << "NetworkIO: FFT input max amplitude:" << maxInput;

    fftw_execute(plan);

    // Shift FFT and find peak
    float spectrum[size];
    int half = size / 2;
    float maxMag = 0.0f;
    int peakIdx = 0;
    for (int i = 0; i < size; ++i) {
        int idx = (i + half) % size;
        float mag = sqrtf(out[idx][0] * out[idx][0] + out[idx][1] * out[idx][1]);
        spectrum[i] = 20.0f * log10f(std::max(mag, 1e-10f));
        if (mag > maxMag) {
            maxMag = mag;
            peakIdx = i;
        }
    }
    qDebug() << "NetworkIO: Peak at index:" << peakIdx << "mag:" << maxMag;

    float minVal = *std::min_element(spectrum, spectrum + size);
    float maxVal = *std::max_element(spectrum, spectrum + size);
    qDebug() << "NetworkIO: Emitting spectrum, size:" << size
             << "min:" << minVal << "max:" << maxVal;
    emit spectrumDataAvailable(spectrum, size);

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
}
