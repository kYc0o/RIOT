
GCC=gcc
CFLAGS=-std=gnu99

SERVER_SRC= server.c requests.c list.c
SERVER_OBJ= $(SERVER_SRC:%.c=%.o)

all:server

server: $(SERVER_OBJ)
	echo $(SERVER_OBJ)
	$(GCC) $(CFLAGS) -o server $(SERVER_OBJ)
	
#%.o: %.c
#	echo "mierda $(<)"
#	$(GCC) $(CFLAGS) -c $<
	
