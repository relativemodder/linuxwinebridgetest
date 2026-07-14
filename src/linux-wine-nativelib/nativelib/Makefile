CC      = winegcc
CFLAGS  = -D__WINESRC__
LIBS    = -ldl -lntdll
SRCS    = dllmain.c
SPEC    = nativelib.spec
TARGET  = nativelib

DEST    = ../../../resources

all: $(TARGET).dll.so

$(TARGET).dll.so: $(SRCS) $(SPEC)
	$(CC) -shared -o $(TARGET).dll $(CFLAGS) $(SRCS) $(SPEC) $(LIBS)
	mv $(TARGET).dll.so $(DEST)/$(TARGET).dll.so

clean:
	rm -f $(TARGET).dll $(TARGET).dll.so *.o
	rm -f $(DEST)/$(TARGET).dll.so

.PHONY: all clean
