UNAME := $(shell uname)

PROJECT = OpenGLExample

CCC = g++

ifeq ($(UNAME), Darwin)

CCFLAGS = -I"../../Public Headers" -I/usr/X11R6/include -I"$(CG_INCPATH)" -DMAC -O3

LDFLAGS = -L"../../lib/macos" -L/usr/X11R6/lib -L"$(CG_LIB_PATH)"

LIBS = -lSilverLining -framework Cg -framework GLUT -framework OpenGL

else

CCFLAGS = -I"../../Public Headers" -DLINUX -DGLUT_DISABLE_ATEXIT_HACK -O3

LDFLAGS = -L"../../lib/linux"

LIBS = -lSilverLining -lGL -lglut -lpthread -lCg -l CgGL

endif

SRC = OpenGLExample.cpp

OBJ = OpenGLExample.o

all: $(PROJECT)

$(PROJECT): $(OBJ)
	$(CCC) $(CCFLAGS) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

OpenGLExample.o : OpenGLExample.cpp
	$(CCC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(PROJECT) Makefile.bak
