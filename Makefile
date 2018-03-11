CC = clang
CFLAGS = -isystem ./include
LDFLAGS = -Llib -rpath '$$ORIGIN/lib'
LDFLAGS += -lphidget22

all: clean build

build: dynpos

clean:
	rm -f *.o
