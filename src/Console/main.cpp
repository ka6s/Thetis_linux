#include <QApplication>
#include <Console.h>
#include <TCIServer.h>
#include <CWKeyer.h>
#include <Audio.h>
#include <Setup.h>
#include <Display.h>
#include <Filter.h>
#include <VFO.h>
#include <QDebug>

int main(int argc, char* argv[]) {
    qDebug() << "Starting Thetis_linux";
    QApplication app(argc, argv);
    qDebug() << "QApplication initialized";

    Console console;
    qDebug() << "Console initialized";

    // Initialize TCP server
    TCPIPtciSocketListener server(40000, &console);
    server.Start();
    qDebug() << "TCP server started on port 40000";

    // Initialize CW keyer
    CWKeyer keyer(&console);
    keyer.setIambic(true);
    keyer.setKeyerSpeed(20);
    keyer.setBreakIn(true);
    keyer.setBreakInDelay(50);
    keyer.key(true);
    QThread::msleep(100);
    keyer.key(false);
    qDebug() << "CW keyer initialized and tested";

    // Initialize audio
    Audio audio(&console);
    if (audio.initialize(48000, 256)) {
        audio.start();
        qDebug() << "Audio initialized and started";
    } else {
        qDebug() << "Audio initialization failed";
    }

    // Initialize filter
    Filter filter(&console);
    filter.setFilterType("Bandpass");
    filter.setFilterBandwidth(3000);
    qDebug() << "Filter initialized";

    // Initialize VFO
    VFO vfo(&console);
    vfo.setFrequency(7000000);
    vfo.setVFOMode("VFO A");
    vfo.setStepSize(100);
    qDebug() << "VFO initialized";

    // Show Setup dialog
    Setup setup(&console);
    setup.show();
    qDebug() << "Setup dialog shown";

    // Show Display widget
    Display display(&console, nullptr);
    display.show();
    qDebug() << "Display widget shown";

    // CAT initialization skipped
    qDebug() << "No serial device connected, skipping CAT initialization";

    qDebug() << "Entering event loop";
    return app.exec();
}
