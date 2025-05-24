# Thetis_linux Checkpoint - May 24, 2025

## Project Overview
This checkpoint captures the state of the `Thetis_linux` project, a Linux port of a software-defined radio (SDR) application derived from Thetis (Windows). The project compiles successfully on Ubuntu (x86_64) and runs without segmentation faults, displaying a Setup dialog (for configuring sample rate, mode, and filter settings) and a Display widget (showing a simulated sine wave spectrum).

## Current Components
The project includes the following functional components, translated from C# to C++:
- **Console**: Manages radio state (frequency, mode, sample rate) and WDSP integration (`OpenChannel`, `SetRXAMode`, `SetInputSamplerate`).
- **TCIServer**: Handles TCP-based Transceiver Control Interface (TCI) commands (e.g., `FREQ7000000`, `MODE2`) on port 40000.
- **CWKeyer**: Configures and tests CW (Morse code) keying parameters (iambic mode, speed, break-in).
- **Audio**: Initializes PortAudio for audio output at 48000 Hz, 256 samples.
- **CAT**: Placeholder for serial port-based rig control (skipped due to no serial device).
- **Setup**: Qt-based GUI dialog with General (Sample Rate, Mode) and DSP (Filter Type, Bandwidth) tabs.
- **Display**: Qt-based widget showing a simulated green sine wave spectrum (256 samples, updated every 50ms).
- **Filter**: Configures DSP filter settings (Bandpass, LowPass, HighPass, Notch) and bandwidth.

**Excluded Components** (not yet incorporated):
- `VFO.cs`, `AGC.cs`, `WaveControl.cs`, `NetworkIO.cs` (proposed but not added due to compilation issues).

## Project Structure
Thetis_linux/
├── include/
│   ├── Console.h
│   ├── TCIServer.h
│   ├── CWKeyer.h
│   ├── Audio.h
│   ├── CAT.h
│   ├── Setup.h
│   ├── Display.h
│   ├── Filter.h
├── src/Console/
│   ├── main.cpp
│   ├── console.cpp
│   ├── TCIServer.cpp
│   ├── cwkeyer.cpp
│   ├── audio.cpp
│   ├── cat.cpp
│   ├── setup.cpp
│   ├── display.cpp
│   ├── filter.cpp
├── Makefile
├── build/ (generated during build)
├── ../wdsp/libwdsp.a (external WDSP library)

include/TCIServer.h, src/Console/TCIServer.cpp
Implements TCPIPtciSocketListener for TCI commands on port 40000.

Expected to handle FREQ and MODE commands, forwarding to Console::tci_cmd.

Action: Please confirm presence and contents. If missing, provide the files or request their recreation.

include/CWKeyer.h, src/Console/cwkeyer.cpp
Implements CW keyer settings (iambic, speed, break-in, delay) and test keying.

Action: Please confirm presence and contents. If missing, provide or request.

include/Audio.h, src/Console/audio.cpp
Initializes PortAudio for 48000 Hz, 256 samples; starts audio stream.

Action: Please confirm presence and contents. If missing, provide or request.

include/CAT.h, src/Console/cat.cpp
Placeholder for serial port rig control, currently skipped.

Action: Please confirm presence and contents. If missing, provide or request.

Dependencies
Ubuntu Packages:
qt5-default, libqt5core5a, libqt5network5, libqt5widgets5, libqt5serialport5-dev

portaudio19-dev, libfftw3-dev

build-essential, g++, make

WDSP Library:
Located at ../wdsp/libwdsp.a.

Provides OpenChannel, create_anbEXT, destroy_anbEXT, SetRXAMode, SetInputSamplerate, SetEXTANBSamplerate.

Install dependencies:
bash

sudo apt update
sudo apt install qt5-default libqt5core5a libqt5network5 libqt5widgets5 libqt5serialport5-dev portaudio19-dev libfftw3-dev build-essential g++ make

Current Functionality
Compilation: Successful with make (as of May 24, 2025).

Runtime:
Setup dialog displays with:
General tab: Sample Rate (48000, 96000, 192000, 384000 Hz), Mode (LSB, USB, CW, AM).

DSP tab: Filter Type (Bandpass, LowPass, HighPass, Notch), Filter Bandwidth (100–10000 Hz).

Apply/Cancel buttons; settings logged to terminal.

Display widget shows a green sine wave spectrum (simulated, 256 samples, updated every 50ms).

TCP server responds to commands on port 40000 (e.g., FREQ7000000 → ACK:FREQ7000000).

CW keyer initializes and tests keying.

Audio initializes at 48000 Hz, 256 samples (no actual output yet).

CAT skipped (no serial device).

No segmentation faults (fixed via OpenChannel in console.cpp).

Known Issues
Filter: setFilterType and setFilterBandwidth are placeholders; no WDSP filter implementation yet.

Display: Uses simulated sine wave data; no real IQ data integration.

Audio: Initializes but lacks actual audio output.

TCI Commands: Limited to FREQ and MODE; may need expansion.

CAT: Skipped; requires serial device for testing.


