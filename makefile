TARGET = opt
LDFLAGS = 
CPP = g++
CPPFLAGS = -std=c++11 -g -Isrc

all: dirs $(TARGET)

csc:
	cd csc_src && make
.PHONY: csc

src = $(wildcard src/*.cpp)
obj = $(patsubst src/%,build/%,$(src:.cpp=.o))
dep = $(obj:.o=.dep)

dirs:
	@mkdir -p build

$(TARGET): $(obj)
	$(CPP) -o $(TARGET) $^ $(LDFLAGS)


build/%.dep: src/%.cpp
	$(CPP) $(CPPFLAGS) $< -MM -MT $(@:.dep=.o) >$@

-include $(dep)

build/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf build/* csc $(TARGET)

.PHONY: dirs clean
