#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "command.h"
#include "command_registry.h"

typedef struct {
	const CommandDefinition* command;
	float score;
} ClassificationCandidate;

#define CLASSIFIER_MAX_CANDIDATES 5

typedef struct {
	ClassificationCandidate candidates[CLASSIFIER_MAX_CANDIDATES];
} ClassificationResult;

typedef struct {
	void* data;
} Classifier;

void classifierInit(Classifier* classifier, const CommandRegistry* registry);
void classifierDestroy(Classifier* classifier);
ClassificationResult classifierClassify(Classifier* classifier, const char* text);

#endif
