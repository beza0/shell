CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -g
LIBS = `pkg-config --libs gtk+-3.0`

OBJS = model.o controller.o view.o

all: shell_app

shell_app: $(OBJS)
	$(CC) -o shell_app $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o shell_app
