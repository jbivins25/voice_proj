#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "command.h"
#include "command_registry.h"

#include <stdbool.h>
#include <stddef.h>

#define CLASSIFIER_MAX_CANDIDATES 5

typedef struct {
	const CommandDefinition* command;
	float score;
} ClassificationCandidate;

typedef struct {
	ClassificationCandidate candidates[CLASSIFIER_MAX_CANDIDATES];
	size_t count;
} ClassificationResult;

typedef struct {
	void* data;
} Classifier;

bool classifierInit(Classifier* classifier, const CommandRegistry* registry);
void classifierDestroy(Classifier* classifier);
ClassificationResult classifierClassify(Classifier* classifier, const char* text);
bool classifierIsConfident(const ClassificationResult* result);
bool classifierShouldClarify(const ClassificationResult* result);

#endif
