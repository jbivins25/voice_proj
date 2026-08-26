#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "command.h"

typedef struct {
	const char* name;
	const char* value;
} Argument;

typedef struct {
	Argument* values;
	size_t count;
} CommandArguments;

bool argumentsExtract(const CommandDefinition* command, const char* text, CommandArguments* arguments);
bool argumentsValidate(const CommandDefinition* command, const CommandArguments* arguments);
void argumentsDestroy(CommandArguments* arguments);

#endif
