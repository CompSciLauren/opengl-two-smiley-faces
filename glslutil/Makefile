CPP = g++ -std=c++11
C_FLAGS = -fPIC -O -c -DGL_GLEXT_PROTOTYPES

LINK = g++ -fPIC

OBJS = ShaderIF.o

libglsl.so: $(OBJS)
	$(LINK) -shared -o libglsl.so $(OBJS)
	cp libglsl.so ../lib/

ShaderIF.o: ShaderIF.h ShaderIF.c++
	$(CPP) $(C_FLAGS) ShaderIF.c++
