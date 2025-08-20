# Makefile for lib9.a library

CC = gcc
CFLAGS = -Wall -Wextra -O0 -Iinclude -g
AR = ar
ARFLAGS = rcs

# Library name
LIB = lib9.a

# Source directories
FMT_DIR = fmt
UTF_DIR = utf
BIO_DIR = bio

# Source files
FMT_SRCS = $(wildcard $(FMT_DIR)/*.c)
UTF_SRCS = $(wildcard $(UTF_DIR)/*.c)
BIO_SRCS = $(wildcard $(BIO_DIR)/*.c)
SRCS = $(FMT_SRCS) $(UTF_SRCS) $(BIO_SRCS)

# Object files
FMT_OBJS = $(FMT_SRCS:.c=.o)
UTF_OBJS = $(UTF_SRCS:.c=.o)
BIO_OBJS = $(BIO_SRCS:.c=.o)
OBJS = $(FMT_OBJS) $(UTF_OBJS) $(BIO_OBJS)

# Default target
all: $(LIB)

# Build the library
$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(LIB)

# Phony targets
.PHONY: all clean
