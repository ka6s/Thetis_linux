#include <QApplication>
#include <QDebug>
#include <Console.h>
#include <WaveControl.h>
#include <Radio.h>
#include <NetworkIO.h>
#include <Display.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << "ThetisCpp starting";

    // Initialize components
    Console console;
    Radio radio(&console);
    NetworkIO networkIO(&console);
    WaveControl waveControl(&console);
    Display display(&console, nullptr);

    // Connect NetworkIO to Display for spectrum updates
    bool connected = QObject::connect(&networkIO, &NetworkIO::spectrumDataAvailable,
                                      &display, &Display::updateSpectrum);
    qDebug() << "NetworkIO to Display connection:" << (connected ? "Success" : "Failed");

    // Show main components
    waveControl.show();
    display.show();

    // Configure initial settings
    radio.setFrequency(14.0e6); // 14 MHz
    display.setCenterFrequency(14.0e6);
    display.setBandwidth(96000); // 96 kHz
    networkIO.setHost("localhost", 50001);
    networkIO.start();

    qDebug() << "Main application loop starting";
    return app.exec();
}
