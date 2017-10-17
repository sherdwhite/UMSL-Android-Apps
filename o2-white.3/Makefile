CC	= gcc
CFLAGS = -g
TARGET1	= oss
TARGET2	= user
OBJS1	= oss.o
OBJS2	= user.o

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LIBS)

$(TARGET2): $(OBJS2)
	$(CC) -o $@ $(OBJS2) $(LIBS)

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $<

.PHONY: clean

clean:
	/bin/rm -f core *.o $(TARGET1) $(TARGET2)
