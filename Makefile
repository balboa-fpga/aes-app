AES_SRCS := aes.c
AES_OBJS := $(AES_SRCS:.c=.o)

balboadir := /opt/balboa

CFLAGS := -O2 -Wall -g -I$(balboadir)/include
LDFLAGS := -L$(balboadir)/lib

all: aes

aes: $(AES_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o aes $(AES_OBJS) -lbalboa

test:
	true

clean:
	rm -f aes *.o
