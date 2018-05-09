CC = clang
CFLAGS = -isystem ./include
CFLAGS += -I../include
CFLAGS += -g
LDFLAGS = -L../lib -rpath '$$ORIGIN/../lib'
LDFLAGS += -lpthread
LDFLAGS += -lphidget22
LDFLAGS += -lGL -lglut -lGLEW -lGLU

all: clean build

build: dynpos

dynpos: linked_list.o gl_framework.o phidget_tools.o voltage_input.o servo.o pid_visuals.o controllers.o log_io.o

clean:
	rm -f *.o
