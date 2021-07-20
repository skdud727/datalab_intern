CC		= g++
LD		= g++

INCDIR	= -I../src/ \
	  -I../src/kvs \

SRCDIR			= ../src
KVSDIR			= ../src/kvs

VPATH	= $(SRCDIR) $(KVSDIR)

CFLAGS	= $(INCDIR) \
		  -Wall \
		  -O2 \
		  -g \
		  -std=c++11 \
		  -MMD \
		  -MP \
		  -fPIC \
		  -march=native \
		  -fsanitize=address \
#		  -DLIST \
#		  -DTREE \
#		  -DHASH \

CXX_FLAGS = \
			-O2 \
			-g \
			-std=c++11 \
			-MMD \
			-MP \
			-Wall \
			-fPIC \
			-march=native \
			

LIBS	=	-lpthread \


TARGET	= my_kvs

SRCS	= $(foreach dir, .. $(SRCDIR) $(KVSDIR), $(wildcard $(dir)/*.c))
SRCS	:= $(notdir $(SRCS))

OBJS	= $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $^ -o $(TARGET) $(CFLAGS) $(LIBS)
	mv $(TARGET) ../

%o: %c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)
	rm -f ../$(TARGET)

depend: $(SRCS)
	$(CC) -M $(CFLAGS) $^ > $@

-include depend
