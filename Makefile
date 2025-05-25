# Compiler and tools
CXX = g++
MOC = moc
UIC = uic

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -fPIC -g $(shell pkg-config --cflags Qt5Core Qt5Network Qt5SerialPort Qt5Widgets portaudio-2.0) -I$(INCLUDE_DIR) -I../wdsp
LDFLAGS = $(shell pkg-config --libs Qt5Core Qt5Network Qt5SerialPort Qt5Widgets portaudio-2.0) -L../wdsp -lwdsp -lfftw3

# Directories
SRC_DIR = src/Console
INCLUDE_DIR = include
BUILD_DIR = build
LIB_DIR = lib
UI_DIR = ui

# Source and header files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/console.cpp \
       $(SRC_DIR)/display.cpp \
       $(SRC_DIR)/networkio.cpp \
       $(SRC_DIR)/setup.cpp \
       $(SRC_DIR)/vfo.cpp \
       $(SRC_DIR)/filter.cpp \
       $(SRC_DIR)/audio.cpp \
       $(SRC_DIR)/wavecontrol.cpp \
       $(SRC_DIR)/waveoptions.cpp \
       $(SRC_DIR)/TCIServer.cpp \
       $(SRC_DIR)/cwkeyer.cpp \
       $(SRC_DIR)/cat.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MOC_SRCS = $(INCLUDE_DIR)/TCIServer.h \
           $(INCLUDE_DIR)/CWKeyer.h \
           $(INCLUDE_DIR)/Audio.h \
           $(INCLUDE_DIR)/CAT.h \
           $(INCLUDE_DIR)/Console.h \
           $(INCLUDE_DIR)/Setup.h \
           $(INCLUDE_DIR)/Display.h \
           $(INCLUDE_DIR)/Filter.h \
           $(INCLUDE_DIR)/VFO.h \
           $(INCLUDE_DIR)/NetworkIO.h \
           $(INCLUDE_DIR)/WaveControl.h \
           $(INCLUDE_DIR)/WaveOptions.h
MOC_OBJS = $(MOC_SRCS:$(INCLUDE_DIR)/%.h=$(BUILD_DIR)/moc_%.o)

# UI files (add if you have .ui files from Qt Designer)
UI_FILES = # $(UI_DIR)/setup.ui $(UI_DIR)/wavecontrol.ui
UI_HEADERS = $(UI_FILES:$(UI_DIR)/%.ui=$(INCLUDE_DIR)/ui_%.h)

# Target executable
TARGET = Thetis_linux

# Default target
all: $(BUILD_DIR) $(UI_HEADERS) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(TARGET): $(OBJS) $(MOC_OBJS)
	$(CXX) $(OBJS) $(MOC_OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(UI_HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate and compile moc files for Qt classes
$(BUILD_DIR)/moc_%.o: $(BUILD_DIR)/moc_%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate moc files
$(BUILD_DIR)/moc_%.cpp: $(INCLUDE_DIR)/%.h
	$(MOC) $< -o $@

# Generate UI headers from .ui files
$(INCLUDE_DIR)/ui_%.h: $(UI_DIR)/%.ui
	$(UIC) $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/* $(INCLUDE_DIR)/ui_*.h $(TARGET)

# Phony targets
.PHONY: all clean
