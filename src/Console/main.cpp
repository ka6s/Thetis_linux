#include <QApplication>
#include <Console.h>
#include <TCIServer.h>
#include <CWKeyer.h>
#include <Audio.h>
#include <Setup.h>
#include <Display.h>
#include <Filter.h>
#include <VFO.h>
#include <NetworkIO.h>
#include <WaveControl.h>
#include <QDebug>
#include <QTimer>

int main(int argc, char* argv[]) {
    qDebug() << "Starting Thetis_linux";
    QApplication app(argc, argv);
    qDebug() << "QApplication initialized";

    Console console;
    qDebug() << "Console initialized";

    TCPIPtciSocketListener server(40000, &console);
    server.Start();
    qDebug() << "TCP server started on port 40000";

    CWKeyer keyer(&console);
    keyer.setIambic(true);
    keyer.setKeyerSpeed(20);
    keyer.setBreakIn(true);
    keyer.setBreakInDelay(50);
    keyer.key(true);
    QThread::msleep(100);
    keyer.key(false);
    qDebug() << "CW keyer initialized and tested";

    Audio audio(&console);
    if (audio.initialize(48000, 256)) {
        audio.start();
        qDebug() << "Audio initialized and started";
    } else {
        qDebug() << "Audio initialization failed";
    }

    Filter filter(&console);
    filter.setFilterType("Bandpass");
    filter.setFilterBandwidth(3000);
    qDebug() << "Filter initialized";

    VFO vfo(&console);
    vfo.setFrequency(7000000);
    vfo.setVFOMode("VFO A");
    vfo.setStepSize(100);
    qDebug() << "VFO initialized";

    NetworkIO networkIO(&console);
    if (networkIO.initialize("192.168.1.100", 1024)) {
        networkIO.sendCommand("START");
        qDebug() << "NetworkIO initialized and START command sent";
    } else {
        qDebug() << "NetworkIO initialization failed";
    }

    QTimer iqTimer(&app);
    QObject::connect(&iqTimer, &QTimer::timeout, &networkIO, &NetworkIO::simulateIQData);
    iqTimer.start(1000);

    Setup setup(&console, &filter, &vfo);
    setup.show();
    qDebug() << "Setup dialog shown";

    Display display(&console, &networkIO);
    display.show();
    qDebug() << "Display widget shown";

    WaveControl waveControl(&console);
    waveControl.show();
    qDebug() << "WaveControl dialog shown";

    qDebug() << "No serial device connected, skipping CAT initialization";

    qDebug() << "Entering event loop";
    return app.exec();
}
