ifeq ($(OS),Windows_NT)
  SDL3_PLATFORM = windows
  SDL3_INCLUDE = -Ideps/SDL3/$(SDL3_PLATFORM)/include
  SDL3_LIB = -Ldeps/SDL3/$(SDL3_PLATFORM)/lib/x64 -lSDL3
else
  SDL3_PLATFORM = unix
  SDL3_SRC = deps/SDL3/$(SDL3_PLATFORM)
  SDL3_INSTALL = build/SDL3-install
  SDL3_INCLUDE = -I$(SDL3_INSTALL)/include
  SDL3_LIB = -L$(SDL3_INSTALL)/lib -lSDL3
endif

UNAME := $(shell uname)
TARGET = build/retrograde
SRCS = main.cpp
OBJS = $(addprefix build/, $(SRCS:.cpp=.o))
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra $(SDL3_INCLUDE)
LDFLAGS = $(SDL3_LIB)

ifeq ($(UNAME), Darwin)
  CXXFLAGS += -arch arm64 -arch x86_64
  LDFLAGS += -arch arm64 -arch x86_64 \
		-framework Cocoa \
    -framework AVFoundation \
    -framework CoreVideo \
    -framework CoreMedia \
    -framework CoreAudio \
    -framework AudioToolbox \
    -framework GameController \
    -framework IOKit \
    -framework QuartzCore \
    -framework Metal \
    -framework CoreHaptics \
    -framework ForceFeedback \
    -framework Carbon \
		-framework UniformTypeIdentifiers
endif

ifeq ($(OS),Windows_NT)
all: $(TARGET)
else
all: $(SDL3_INSTALL)/lib/libSDL3.a $(TARGET)
endif

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SDL3_INSTALL)/lib/libSDL3.a:
	@echo ">>> Building SDL3 using CMake..."
	@mkdir -p $(SDL3_SRC)/build
	cd $(SDL3_SRC)/build && env -u LDFLAGS -u CFLAGS -u CXXFLAGS cmake .. \
		-DCMAKE_INSTALL_PREFIX=$(abspath $(SDL3_INSTALL)) \
		-DCMAKE_BUILD_TYPE=Release \
		-DSDL_STATIC=ON -DSDL_SHARED=OFF \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
	cd $(SDL3_SRC)/build && cmake --build . --target install -j$(shell sysctl -n hw.logicalcpu 2>/dev/null || nproc)
	cp -f $(SDL3_SRC)/build/compile_commands.json ./compile_commands.json
	@echo ">>> SDL3 installed to $(SDL3_INSTALL)"

clean:
	rm -rf build

distclean: clean
	rm -rf deps/SDL3/$(SDL3_PLATFORM)/build
