CSRCS=CloudsDrawable.cpp osgviewer.cpp SkyDrawable.cpp

CFLAGS = -c -O3 -DLINUX 

INCLUDE = -I. -I/usr/include/ -I/usr/include/X11/ -I/usr/local/include/GL 
INCOSG = -I$(OSG_ROOT)/include -I"../../Public Headers"
LDLIBS = -lSilverLining -lm -ldl -lGL -lGLU -lpthread -lglut -lXi -lXmu -lXext -lXext -lX11 -lCg -lCgGL

LDFLAGS =  -L. -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib -L"../../lib"
LDOSG = -L$(OSG_ROOT)/lib -losg -losgViewer -losgSim
COBJS=$(patsubst %.cpp,%.o,$(CSRCS))

CC = g++

EXE=osex

all: $(EXE)

$(EXE): $(COBJS) 
	$(CC) -o$(EXE) $(COBJS) $(INCLUDE) $(INCOSG) $(LDFLAGS) $(LDLIBS) $(LDOSG)


$(COBJS): %.o : %.cpp 
	$(CC) $(CFLAGS) $(INCOSG) -o $@ $< 

clean:
	/bin/rm osex *.o


