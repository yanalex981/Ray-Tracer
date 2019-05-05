CXX = g++
INCPATH = /usr/include/ 3rdparty/ 3rdparty/glm/
# LIBPATH = /lib/x86_64-linux-gnu/ /usr/local/lib/
LIBPATH = /usr/lib/
LIBS = sfml-graphics sfml-window sfml-system GL X11 Xrandr
CXXFLAGS = -Wextra -Wall -Wno-missing-braces -mavx -march=native -std=c++17 -O3 $(addprefix -I,$(INCPATH))
LINKER = $(addprefix -L,$(LIBPATH)) $(addprefix -l,$(LIBS))
# LDFLAGS=-Wl,-R -Wl,/usr/local/lib/

tracer: main.o plane.o cone.o sphere.o
	$(CXX) $(CXXFLAGS) $^ $(LINKER) -o $@ -pthread -ludev -lXrandr

3rdparty/json/:
	git clone --recursive https://github.com/nlohmann/json $@

3rdparty/imgui/:
	git clone --recursive https://github.com/ocornut/imgui $@

3rdparty/glm/:
	git clone --recursive https://github.com/g-truc/glm $@

sphere.o: surface.hpp sphere.hpp sphere.cpp material.hpp
plane.o: surface.hpp plane.hpp plane.cpp material.hpp
cone.o: surface.hpp cone.hpp cone.cpp material.hpp
# vector.o: vector.cpp vector.hpp
main.o: main.cpp light.hpp surface.hpp plane.hpp cone.hpp material.hpp sphere.hpp

.PHONY: clean
clean:
	rm -r tracer *.dSYM *.o
