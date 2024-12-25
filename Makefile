# Makefile

# Compiler and Flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# Directories
SRC_DIR := src
VENDOR_DIRS := vendor/imgui-docking vendor/stb # Removed vendor/glad
BUILD_DIR := build

# Include Directories
# Add GLFW include paths
GLFW_INCLUDE := C:/libraries/glfw/include
INCLUDE_DIRS := $(SRC_DIR) $(VENDOR_DIRS) $(GLFW_INCLUDE) vendor/stb/include # Removed vendor/glad/include
INCLUDES := $(addprefix -I, $(INCLUDE_DIRS))

# Update compiler flags with include paths
CXXFLAGS += $(INCLUDES)

# Source Files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
VENDOR_SRC := $(foreach dir, $(VENDOR_DIRS), $(wildcard $(dir)/*.cpp))
STB_SRC := $(wildcard vendor/stb/src/*.cpp) # If stb has .cpp files
ALL_SRC := $(SRC_FILES) $(VENDOR_SRC) $(STB_SRC)

# Object Files
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(ALL_SRC))
# Removed GLAD object files
# OBJ_FILES += $(patsubst %.c, $(BUILD_DIR)/%.o, $(GLAD_SRC)) # Removed

# Target executable name
TARGET := TesseractEngine.exe

# Libraries
LIBS := -LC:/libraries/glfw/lib -lglfw3 -lopengl32 -lgdi32 -limm32 -lole32 -loleaut32 -luuid -lwinmm

# Phony Targets
.PHONY: all clean copy_assets

# Default target
all: copy_assets $(TARGET)

# Copy assets/fonts to build/assets/fonts
copy_assets:
	@echo Copying font assets...
	xcopy /E /I /Y "assets\fonts" "$(BUILD_DIR)\assets\fonts" >nul

# Link all object files to create the executable
$(TARGET): $(OBJ_FILES)
	@echo Linking $@...
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Pattern rule to compile .cpp files to .o files
$(BUILD_DIR)/%.o: %.cpp
	@mkdir "$(dir $@)" >nul 2>&1 || echo Directory exists
	@echo Compiling $<...
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo Cleaning up...
	rmdir /s /q "$(BUILD_DIR)"
	del /q "$(TARGET)"
