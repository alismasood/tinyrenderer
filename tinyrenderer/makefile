OBJS = main.o tgaimage.o model.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

tinyrenderer : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o tinyrenderer

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

tgaimage.o: tgaimage.h tgaimage.cpp
	$(CC) $(CFLAGS) tgaimage.cpp

model.o: model.h model.cpp geometry.h
	$(CC) $(CFLAGS) model.cpp

clean:
	\rm *.o *~ tinyrenderer
