CC=gcc
CXX=g++
RM=rm
WARNINGS := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-declarations -Wredundant-decls \
			-Winline -Wno-long-long -Wuninitialized -Wconversion
CWARNINGS := $(WARNINGS) -Wmissing-prototypes -Wnested-externs \
			-Wstrict-prototypes
CXXWARNINGS := $(WARNINGS)
CFLAGS := -g -fPIC -std=c99 $(CWARNINGS)
CXXFLAGS := -g $(CXXWARNINGS)

CLIENTOBJS=client.o
SERVEROBJS=server.o

all: server client

client: $(CLIENTOBJS)
	gcc -o client $(CLIENTOBJS)

server: $(SERVEROBJS)
	gcc -o server $(SERVEROBJS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -fPIC -MMD -MP -c $< -o $@

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -fPIC -MMD -MP -c $< -o $@

clean:
	-$(RM) -f $(CLIENTOBJS) client $(SERVEROBJS) server *.d

.PHONY: all clean
