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
	const char* name;
	ParameterType type;
	bool required;

	const char* const* enumValues;
	size_t enumValueCount;
} CommandParameter;

typedef struct {
	const char* id;
	const char* name;
	const char* description;
	// Examples of requests that should map to respective command
	const char* const *examples;
	size_t exampleCount;
	const char* const *keywords;
	size_t keywordCount;
	const CommandParameter* parameters;
	size_t parameterCount;
	bool requiresConfirmation;
} CommandDefinition;

bool commandMatchesName(const CommandDefinition* command, const char* name);
bool commandValidate(const CommandDefinition* command);

#endif
