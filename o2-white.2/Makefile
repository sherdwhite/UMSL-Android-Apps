CC	= g++

TARGET1	= palin

OBJS1	= palin.o


all: $(TARGET1) 

$(TARGET1): $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LIBS)


.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $<

.PHONY: clean

clean:
	/bin/rm -f core *.o $(TARGET1) 
