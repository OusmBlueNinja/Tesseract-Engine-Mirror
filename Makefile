# Makefile

# Compiler and Flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g -DDEBUG

# Directories
SRC_DIR := src
VENDOR_DIRS := vendor/imgui-docking vendor/stb
BUILD_DIR := build

# Include Directories
GLFW_INCLUDE := C:/libraries/glfw/include
INCLUDE_DIRS := $(SRC_DIR) $(VENDOR_DIRS) $(GLFW_INCLUDE) vendor/stb/include C:\msys64\mingw64\include vendor/lua vendor/gcml
INCLUDES := $(addprefix -I, $(INCLUDE_DIRS))

# Update compiler flags with include paths
CXXFLAGS += $(INCLUDES)

# -------------------------------------------------------------------------
# Source Files
#  1) Recursively gather *.cpp in src (including subfolders).
#  2) Gather *.cpp from vendor/imgui-docking, vendor/stb, etc.
# -------------------------------------------------------------------------
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) \
             $(wildcard $(SRC_DIR)/**/*.cpp)

VENDOR_SRC := $(foreach dir, $(VENDOR_DIRS), $(wildcard $(dir)/*.cpp))
STB_SRC := $(wildcard vendor/stb/src/*.cpp) # If stb has .cpp files
ALL_SRC := $(SRC_FILES) $(VENDOR_SRC) $(STB_SRC)

# -------------------------------------------------------------------------
# Object Files
#   Convert each .cpp to a corresponding .o under the build/ directory.
#   For example:
#     src/Engine.cpp -> build/src/Engine.o
#     src/Windows/LoggerWindow.cpp -> build/src/Windows/LoggerWindow.o
# -------------------------------------------------------------------------
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(ALL_SRC))

# Target executable name
TARGET := TesseractEngine.exe

# Libraries
LIBS := -LC:/libraries/glfw/lib -Llib -lglfw3 -lopengl32 -lgdi32 -limm32 -lole32 -loleaut32 -luuid -lwinmm -lglew32 -lglu32 -lyaml-cpp -llua54

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
# Note the mkdir on the $(dir $@) ensures subfolders under build/ exist.
$(BUILD_DIR)/%.o: %.cpp
	@mkdir "$(dir $@)" >nul 2>&1 || echo Directory exists
	@echo Compiling $<...
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo Cleaning up...
	if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	if exist "$(TARGET)" del /q "$(TARGET)"
