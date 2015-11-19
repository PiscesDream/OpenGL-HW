CXX = g++
LD = ld
CFLAGS = -I ../../External/glm-0.9.4.0/
LDFLAGS = 

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

%.exe: %.o
	$(LD) $< -o $@ $(LDFLAGS)
