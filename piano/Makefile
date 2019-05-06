TARGET:=piano

CC:=arm-linux-gcc

CARCS+=$(wildcard *.c)
OBJS+=$(patsubst %.c,%.o,$(CARCS))

LIBS+= -pthread
INCL+= -pthread

%.o:%c
	$(CC) -c $^ -o $@W $(INCL)
	
$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(LIBS)
	cp $@ ~/tftp
	
clean:
	rm $(TARGET)$(OBJS)