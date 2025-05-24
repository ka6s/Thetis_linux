# Compiler and tools
CC = g++
MOC = moc
UIC = uic

# Compiler flags
CFLAGS = -std=c++17 -Wall -fPIC -g $(shell pkg-config --cflags Qt5Core Qt5Network Qt5SerialPort Qt5Widgets portaudio-2.0) -I$(INCLUDE_DIR) -I../wdsp
LDFLAGS = $(shell pkg-config --libs Qt5Core Qt5Network Qt5SerialPort Qt5Widgets portaudio-2.0) -L../wdsp -lwdsp -lfftw3

# Directories
SRC_DIR = src/Console
INCLUDE_DIR = include
BUILD_DIR = build
LIB_DIR = lib

# Source and header files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/TCIServer.cpp $(SRC_DIR)/cwkeyer.cpp $(SRC_DIR)/audio.cpp $(SRC_DIR)/cat.cpp $(SRC_DIR)/console.cpp $(SRC_DIR)/setup.cpp $(SRC_DIR)/display.cpp $(SRC_DIR)/filter.cpp
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MOC_SRCS = $(INCLUDE_DIR)/TCIServer.h $(INCLUDE_DIR)/CWKeyer.h $(INCLUDE_DIR)/Audio.h $(INCLUDE_DIR)/CAT.h $(INCLUDE_DIR)/Console.h $(INCLUDE_DIR)/Setup.h $(INCLUDE_DIR)/Display.h $(INCLUDE_DIR)/Filter.h
MOC_OBJS = $(MOC_SRCS:$(INCLUDE_DIR)/%.h=$(BUILD_DIR)/moc_%.o)

# Target executable
TARGET = Thetis_linux

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(TARGET): $(OBJS) $(MOC_OBJS)
	$(CC) $(OBJS) $(MOC_OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Generate and compile moc files for Qt classes
$(BUILD_DIR)/moc_%.o: $(BUILD_DIR)/moc_%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Generate moc files
$(BUILD_DIR)/moc_%.cpp: $(INCLUDE_DIR)/%.h
	$(MOC) $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/* $(TARGET)

# Phony targets
.PHONY: all clean
