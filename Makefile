# =========================
# Compiler
# =========================
CXX       := g++
CXXFLAGS  := -std=c++17 -O2 -Wall -Wextra

# =========================
# Directories
# =========================
SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := bin

# =========================
# Include paths
# =========================
INCLUDES := \
	-I$(SRC_DIR) \
	-I$(SRC_DIR)/Libs

# =========================
# Libraries
# =========================
LIBS := -lglfw -lGL -ldl -pthread

# =========================
# Sources
# =========================
SOURCES := \
	$(SRC_DIR)/App/Main.cpp \
	$(SRC_DIR)/Core/GLTFLoader/GLTFLoader.cpp \
	$(SRC_DIR)/Core/ImageWriter/ImageWriter.cpp \
	$(SRC_DIR)/Core/Renderer/Renderer.cpp \
	$(SRC_DIR)/Core/Shader/Shader.cpp \
	$(SRC_DIR)/Libs/stb_impl.cpp \
	$(SRC_DIR)/Libs/glad.c

# =========================
# Objects
# =========================
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# =========================
# Output
# =========================
TARGET := $(BIN_DIR)/renderer

# =========================
# Rules
# =========================
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

.PHONY: all clean run
