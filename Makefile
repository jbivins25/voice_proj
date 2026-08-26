CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

LDFLAGS = -lm

TARGET = classifier_test

SOURCES = \
	main.c \
	classifier.c \
	command.c \
	command_registry.c \
	test/test_commands.c

OBJECTS = $(SOURCES:.c=.o)


.PHONY: all clean run


all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@


run: $(TARGET)
	./$(TARGET)


clean:
	rm -f $(OBJECTS) $(TARGET)
