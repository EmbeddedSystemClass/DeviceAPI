CC	= g++

SOLUTION_DIR	= ../../../..

CFLAGS	= -Wall -g -O2 -fomit-frame-pointer -D_REENTRANT -D_OPROS_LINUX_ -fPIC
CFLAGS	+= -I../../CommonLibrary/include
CFLAGS	+= -I../../CommonLibrary/include/device

LDFLAGS	= -shared -W1

SRCS	= TcpSocket.cpp
SRCS	+= TCPSocketLinux.cpp

OBJS	= $(SRCS:.cpp=.o)

TARGET	= TcpSocket.so

all : $(TARGET)

$(TARGET) : $(OBJS) 
	$(CC) -shared -W1 -o $(TARGET) $(OBJS)

%.o : %.cpp
	@echo "C++ Compiling $< ..."
	$(CC) -c $(CFLAGS) -o $@ $<

clean :
	rm -f $(TARGET) *.o

install:
	cp $(TARGET) /lib