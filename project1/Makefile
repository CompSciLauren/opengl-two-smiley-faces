CPP = g++ -std=c++11
INC = -I../glslutil -I../mvcutil -I/usr/local/include -I.

# >>> FOR LINUX, uncomment next few lines; comment out the MAC ones.
C_FLAGS = -fPIC -g -c $(INC) -DGL_GLEXT_PROTOTYPES
GL_LIB_LOC = -L/usr/lib/nvidia-375
GL_LIBRARIES = $(GL_LIB_LOC) -lglfw -lGLU -lGL
MAKE = make
# >>> FOR MAC, uncomment next few lines; comment out previous linux ones.
# C_FLAGS = -fPIC -g -c $(INC) -DGLFW_INCLUDE_GLEXT -DGLFW_INCLUDE_GLCOREARB -DGL_SILENCE_DEPRECATION
# GL_LIBRARIES = -L/usr/local/lib -lglfw -framework OpenGL
# MAKE = make -f MakefileMac
# >>> END: FOR LINUX - FOR MAC

LINK = g++ -fPIC -g
LOCAL_UTIL_LIBRARIES = ../lib/libglsl.so

OBJS = main.o ModelView.o Controller.o GLFWController.o

main: $(OBJS) $(LOCAL_UTIL_LIBRARIES)
	$(LINK) -o main $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(GL_LIBRARIES)

../lib/libglsl.so: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.c++
	(cd ../glslutil; $(MAKE))

main.o: main.c++
	$(CPP) $(C_FLAGS) main.c++
ModelView.o: ModelView.h ModelView.c++
	$(CPP) $(C_FLAGS) ModelView.c++
Controller.o: ../mvcutil/Controller.h ../mvcutil/Controller.c++
	$(CPP) $(C_FLAGS) -DGLFW_INCLUDE_GLU ../mvcutil/Controller.c++
GLFWController.o: ../mvcutil/GLFWController.h ../mvcutil/GLFWController.c++
	$(CPP) $(C_FLAGS) ../mvcutil/GLFWController.c++
