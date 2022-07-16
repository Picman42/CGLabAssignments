# Simple Makefile for 6.837 raycasting assignment
# for g++ on linux platforms

# ===============================================================
# update this list as you add new C++ source files

SRCS 	 	= $(wildcard *.C)

# ===============================================================
# NOTE:  On athena, the Mesa GL libraries are currently located in the
#        mesa_v501 locker.  Type "add mesa_v501" to access these files.

#LIBS            =  -L"D:\Codes\CPP\CGLabAssignments\OpenGL\lib\x64\" -l"glut.lib" 
LIBS		=  -lGL -lGLU -lglut -L/usr/lib
# -lm -lGL -lGLU -lglut -L/usr/X11R6/lib -lXmu -lXi -Xlinker -rpath -Xlinker "D:\Codes\CPP\CGLabAssignments\OpenGL\lib\"
#INCLUDE 	= -I"D:\Codes\CPP\CGLabAssignments\OpenGL\include"
INCLUDE		= -I/usr/include
CC		= g++
EXE	  	= raytracer
OBJS =  $(SRCS:.C=.o)

# ===============================================================
# targets

.PHONY: all depend clean

all: depend $(EXE)

depend:
	$(CC) $(INCLUDE) -E -M $(SRCS) > Makefile.depend

clean: 
	-rm -f *~ *bak *.o  core.* Makefile.depend

# ===============================================================
# compilation rules

$(EXE): $(OBJS)
	$(CC) $(INCLUDE) -o $@ $(OBJS) $(LIBS)

.C.o:
	$(CC) $(INCLUDE) $< -c -o $@

# ===============================================================

-include Makefile.depend

