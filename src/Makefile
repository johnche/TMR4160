CC = clang
CFLAGS = -isystem ./include
CFLAGS += -I../include
LDFLAGS = -L../lib -rpath '$$ORIGIN/../lib'
LDFLAGS += -lpthread
LDFLAGS += -lphidget22
LDFLAGS += -lGL -lglut -lGLEW -lGLU
Target = ../DynamicPositioning

all: clean build

build: dynpos

dynpos: linked_list.o glplot.o phidget_tools.o voltage_input.o servo.o

clean:
	rm -f *.o
