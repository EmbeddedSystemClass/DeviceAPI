CC	= g++

SOLUTION_DIR	= ../../../..

CFLAGS	= -Wall -g -O2 -fomit-frame-pointer -D_REENTRANT -D_OPROS_LINUX_ -fPIC
CFLAGS	+= -I../../CommonLibrary/include
CFLAGS	+= -I../../CommonLibrary/include/device
CFLAGS  += -I/usr/include/opencv

LDFLAGS	= -shared -W1 -L/usr/lib -lcv

SRCS	= OpenCvUsbMultiCamera.cpp

OBJS	= $(SRCS:.cpp=.o)

TARGET	= OpenCvUsbMultiCamera.so

all : $(TARGET)

$(TARGET) : $(OBJS) 
	$(CC) $(LDFLAGS) -shared -W1 -o $(TARGET) $(OBJS) 

%.o : %.cpp
	@echo "C++ Compiling $< ..."
	$(CC) -c $(CFLAGS) -o $@ $<

clean :
	rm -f $(TARGET) *.o

install:
	cp $(TARGET) /lib