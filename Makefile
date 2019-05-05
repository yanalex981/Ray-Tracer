CXX = g++
INCPATH = /usr/local/include/
LIBPATH = /lib/x86_64-linux-gnu/ /usr/local/lib/
LIBS = sfml-graphics-s sfml-window-s sfml-system-s GL X11 Xrandr
CXXFLAGS = -Wextra -Wall -Wno-missing-braces -mavx -march=native -std=c++17 -O3 -I$(INCPATH)
LINKER = $(addprefix -L,$(LIBPATH)) $(addprefix -l,$(LIBS))

tracer: main.o vector.o plane.o cone.o sphere.o
	$(CXX) $(CXXFLAGS) $^ $(LINKER) -o $@ -pthread -ludev -lXrandr

3rdparty/json/:
	git submodule update --recursive

3rdparty/imgui/:
	git submodule update --recursive

3rdparty/glm/:
	git submodule update --recursive

sphere.o: surface.hpp sphere.hpp sphere.cpp matrix_utils.hpp material.hpp matrix.hpp vector.hpp
plane.o: surface.hpp plane.hpp plane.cpp matrix_utils.hpp material.hpp matrix.hpp vector.hpp
cone.o: surface.hpp cone.hpp cone.cpp matrix_utils.hpp material.hpp matrix.hpp vector.hpp
vector.o: vector.cpp vector.hpp
main.o: main.cpp matrix.hpp vector.hpp light.hpp matrix_utils.hpp surface.hpp plane.hpp cone.hpp material.hpp sphere.hpp

.PHONY: clean
clean:
	rm -r tracer *.dSYM *.o

