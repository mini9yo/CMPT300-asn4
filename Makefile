CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRCS = UnixLs.c
OBJS = $(SRCS:.c=.o)
EXEC = UnixLs

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)