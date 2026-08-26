#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	PARAM_STRING,
	PARAM_INTEGER,
	PARAM_FLOAT,
	PARAM_DURATION,
	PARAM_ENUM
} ParameterType;

typedef struct {
	const char* name,
	ParameterType type;
	bool required;
} CommandParameter;

typedef struct {
	const char* name;
	const char* description;
	const char* const *examples;
	size_t exampleCount;
	const char* const *keywords;
	size_t keywordCount;
	const CommandParameter* parameters;
	size_t parameterCount;
	bool requiresConfirmation;
	void* handler;
} CommandDefinition;

#endif
