CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

LDFLAGS = -lm

TARGET = icarus

SOURCES = \
        main.c \
	assistant.c \
        classifier.c \
        command.c \
        command_registry.c \
	formatter.c \
        llm.c \
        llm_local.c \
        llm_remote.c \
	response.c \
	router.c \
	tool.c \
        test/test_commands.c \
	test/test_tools.c

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
