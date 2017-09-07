CC	= g++
// CFLAGS = -g `pkg-config --cflags opencv`
TARGET1	= assignment1
// TARGET2	= part2
// TARGET3 = part3
OBJS1	= assignment1.o
// OBJS2	= part2.o
// OBJS3	= part3.o
// LIBS	= `pkg-config --libs opencv`

all: $(TARGET1) //$(TARGET2) $(TARGET3)

$(TARGET1): $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LIBS)

// $(TARGET2): $(OBJS2)
//	$(CC) -o $@ $(OBJS2) $(LIBS)

//$(TARGET3): $(OBJS3)
//	$(CC) -o $@ $(OBJS3) $(LIBS)

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $<

.PHONY: clean

clean:
	/bin/rm -f core *.o $(TARGET1) //$(TARGET2) $(TARGET3)
