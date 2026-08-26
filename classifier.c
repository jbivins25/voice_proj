#include "classifier.h"

#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INIT_VOCAB_CAP 64
#define INIT_COMMAND_CAP 8
#define INIT_TOKEN_CAP 16

#define MIN_CONFIDENCE 0.6f
#define MIN_SCORE_MARGIN 0.1f

static const char* stopWords[] = {
        "a",
        "an",
        "and",
        "are",
        "am",
        "be",
        "do",
        "does",
        "for",
        "from",
        "how",
        "i",
        "if",
        "in",
        "is",
        "it",
        "me",
        "my",
        "of",
        "on",
        "or",
        "please",
        "tell",
        "the",
        "this",
        "to",
        "what",
        "whats",
        "what's",
        "when",
        "where",
        "who",
        "with",
        "you"
};

#define STOP_WORD_COUNT (sizeof(stopWords) / sizeof(stopWords[0]))

typedef struct {
	char* word;
	size_t documentCount;
} VocabularyEntry;

typedef struct {
	VocabularyEntry* entries;
	size_t count;
	size_t capacity;
} Vocabulary;

typedef struct {
	char** tokens;
	size_t count;
	size_t capacity;
} TokenList;

typedef struct {
	float* values;
	size_t size;
} Vector;

typedef struct {
	Vector vector;
} ExampleVector;

typedef struct {
	const CommandDefinition* command;
	ExampleVector* examples;
	size_t exampleCount;
} CommandVector;

typedef struct {
	Vocabulary vocabulary;

	float* idf;

	CommandVector* commands;
	size_t commandCount;

	size_t totalDocuments;
} ClassifierData;

static char* duplicateString(const char* string) {
	if (string == NULL) return NULL;

	size_t length = strlen(string);

	char* copy = malloc(length + 1);

	if (copy == NULL) return NULL;

	memcpy(copy, string, length + 1);

	return copy;
}

static bool isTokenCharacter(char c) {
	return isalnum((unsigned char)c) != 0 || c == '\'';
}

static bool isStopWord(const char* word) {
	for (size_t i = 0; i < STOP_WORD_COUNT; i++) {
		if (strcmp(word, stopWords[i]) == 0) return true;
	}
	return false;
}

static bool tokenListAdd(TokenList* list, const char* token) {
	if (list->count >= list->capacity) {
		size_t newCap;

		if (list->capacity == 0) newCap = INIT_TOKEN_CAP;
		else newCap = list->capacity * 2;

		char** newTokens = realloc(list->tokens, newCap*sizeof(char*));

		if (newTokens == NULL) return false;

		list->tokens = newTokens;
		list->capacity = newCap;
	}

	char* copy = duplicateString(token);

	if (copy == NULL) return false;

	list->tokens[list->count++] = copy;

	return true;
}

static char* normalizeToken(const char* token, size_t length) {
	char* result = malloc(length + 1);

	if (result == NULL) return NULL;

	for (size_t i = 0; i < length; i++) {
		result[i] = (char)tolower((unsigned char) token[i]);
	}

	result[length] = '\0';

	return result;
}

static const char* normalizeContraction(const char* token) {
	static const struct {
                const char* contraction;
                const char* replacement;
        } contractions[] = {
                { "i'm",       "i" },
                { "im",        "i" },

                { "you're",    "you" },
                { "youre",    "you" },

                { "he's",      "he" },
                { "hes",       "he" },

                { "she's",    "she" },
                { "shes",      "she" },

                { "it's",      "it" },
                { "its",       "it" },

                { "we're",     "we" },
                { "were",      "we" },

                { "they're",   "they" },
                { "theyre",    "they" },

                { "that's",    "that" },
                { "thats",     "that" },

                { "there's",   "there" },
                { "theres",    "there" },

                { "what's",    "what" },
                { "whats",     "what" },

                { "who's",     "who" },
                { "whos",      "who" },

                { "where's",   "where" },
                { "wheres",    "where" },

                { "when's",    "when" },
                { "whens",     "when" },

                { "how's",     "how" },
                { "hows",      "how" },

                { "can't",     "can" },
                { "cant",      "can" },

                { "couldn't",  "could" },
                { "couldnt",   "could" },

                { "don't",     "do" },
                { "dont",      "do" },

                { "doesn't",   "does" },
                { "doesnt",    "does" },

                { "didn't",    "did" },
                { "didnt",     "did" },

                { "won't",     "will" },
                { "wont",      "will" },

                { "wouldn't",  "would" },
                { "wouldnt",   "would" },

                { "shouldn't", "should" },
                { "shouldnt",  "should" },

                { "isn't",     "is" },
                { "isnt",      "is" },

                { "aren't",    "are" },
                { "arent",     "are" },

                { "wasn't",    "was" },
                { "wasnt",     "was" },

                { "weren't",   "were" },
                { "werent",    "were" },

                { "haven't",   "have" },
                { "havent",    "have" },

                { "hasn't",    "has" },
                { "hasnt",     "has" },

                { "hadn't",    "had" },
                { "hadnt",     "had" },

                { "i've",      "i" },
                { "ive",       "i" },

                { "you've",    "you" },
                { "youve",     "you" },

                { "we've",     "we" },
                { "weve",      "we" },

                { "they've",   "they" },
                { "theyve",    "they" },

                { "i'll",      "i" },

                { "you'll",    "you" },
                { "youll",     "you" },

                { "we'll",     "we" },
                { "well",      "we" },

                { "they'll",   "they" },
                { "theyll",    "they" }
        };

        size_t count = sizeof(contractions) / sizeof(contractions[0]);

        for (size_t i = 0; i < count; i++) {
                if (strcmp(token, contractions[i].contraction) == 0) return contractions[i].replacement;
        }

        return token;
}

static void tokenListDestroy(TokenList* list) {
	if (list == NULL) return;

	for (size_t i = 0; i < list->count; i++) {
		free(list->tokens[i]);
	}

	free(list->tokens);

	list->tokens = NULL;
	list->count = 0;
	list->capacity = 0;
}

static TokenList tokenize(const char* text) {
	TokenList result = { .tokens = NULL, .count = 0, .capacity = 0 };

	if (text == NULL) return result;

	size_t length = strlen(text);

	size_t start = 0;

	while (start < length) {
		while (start < length && !isTokenCharacter(text[start])) start++;

		if (start >= length) break;

		size_t end = start;

		while (end < length && isTokenCharacter(text[end])) end++;

		char* token = normalizeToken(&text[start], end - start);

		if (token == NULL) {
			tokenListDestroy(&result);
			return result;
		}

		const char* normalized = normalizeContraction(token);

		if (!isStopWord(normalized)) {
			if (!tokenListAdd(&result, normalized)) {
				free(token);
				tokenListDestroy(&result);
				return result;
			}
		}

		free(token);

		start = end;
	}

	return result;
}

static void vocabularyInit(Vocabulary* vocabulary) {
	vocabulary->entries = NULL;
	vocabulary->count = 0;
	vocabulary->capacity = 0;
}

static void vocabularyDestroy(Vocabulary* vocabulary) {
	if (vocabulary == NULL) return;

	for (size_t i = 0; i < vocabulary->count; i++) {
		free(vocabulary->entries[i].word);
	}

	free(vocabulary->entries);

	vocabulary->entries = NULL;
	vocabulary->count = 0;
	vocabulary->capacity = 0;
}

static size_t vocabularyFind(const Vocabulary* vocabulary, const char* word) {
	for (size_t i = 0; i < vocabulary->count; i++) {
		if (strcmp(vocabulary->entries[i].word, word) == 0) return i;
	}

	return SIZE_MAX;
}

static bool vocabularyAdd(Vocabulary* vocabulary, const char* word) {
	if (vocabularyFind(vocabulary, word) != SIZE_MAX) return true;

	if (vocabulary->count >= vocabulary->capacity) {
		size_t newCap;

		if (vocabulary->capacity == 0) newCap = INIT_VOCAB_CAP;
		else newCap = vocabulary->capacity * 2;

		VocabularyEntry* newEntries = realloc(vocabulary->entries, newCap * sizeof(VocabularyEntry));

		if (newEntries == NULL) return false;

		vocabulary->entries = newEntries;
		vocabulary->capacity = newCap;
	}

	char* copy = duplicateString(word);

	if (copy == NULL) return false;

	vocabulary->entries[vocabulary->count].word = copy;
	vocabulary->entries[vocabulary->count++].documentCount = 0;

	return true;
}

static bool vocabularyProcessExample(Vocabulary* vocabulary, const char* text) {
	TokenList tokens = tokenize(text);

	for (size_t i = 0; i < tokens.count; i++) {
		if (!vocabularyAdd(vocabulary, tokens.tokens[i])) {
			tokenListDestroy(&tokens);
			return false;
		}
	}

	tokenListDestroy(&tokens);
	return true;
}

static bool vocabularyUpdateDocumentFrequency(Vocabulary* vocabulary, const char* text) {
	TokenList tokens = tokenize(text);

	if (tokens.count == 0) {
		tokenListDestroy(&tokens);
		return true;
	}

	for (size_t i = 0; i < tokens.count; i++) {
		bool alreadyCounted = false;

		for (size_t j = 0; j < i; j++) {
			if (strcmp(tokens.tokens[i], tokens.tokens[j]) == 0) {
				alreadyCounted = true;
				break;
			}
		}

		if (alreadyCounted) continue;

		size_t index = vocabularyFind(vocabulary, tokens.tokens[i]);
		if (index != SIZE_MAX) vocabulary->entries[index].documentCount++;
	}

	tokenListDestroy(&tokens);
	return true;
}

static bool buildVocabulary(ClassifierData* data, const CommandRegistry* registry) {
	data->totalDocuments = 0;

	for (size_t i = 0; i < commandRegistryCount(registry); i++) {
		const CommandDefinition* command = commandRegistryGet(registry, i);

		if (command == NULL) continue;

		for (size_t j = 0; j < command->exampleCount; j++) {
			const char* example = command->examples[j];
			if (example == NULL) continue;

			if (!vocabularyProcessExample(&data->vocabulary, example)) return false;
			if (!vocabularyUpdateDocumentFrequency(&data->vocabulary, example)) return false;

			data->totalDocuments++;
		}
	}
	
	return true;
}

static bool calculateIDF(ClassifierData* data) {
	if (data->vocabulary.count == 0) return true;

	data->idf = calloc(data->vocabulary.count, sizeof(float));

	if (data->idf == NULL) return false;

	for (size_t i = 0; i < data->vocabulary.count; i++) {
		size_t documentCount = data->vocabulary.entries[i].documentCount;

		data->idf[i] = logf(((float)data->totalDocuments + 1.0f) / ((float)documentCount + 1.0f)) + 1.0f;
	}

	return true;
}

static void vectorDestroy(Vector* vector) {
	if (vector == NULL) return;

	free(vector->values);
	vector->values = NULL;
	vector->size = 0;
}

static void debugPrintVector(const ClassifierData* data, const Vector* vector) {
        printf("Vector:\n");

        for (size_t i = 0;
             i < vector->size;
             i++) {

                if (vector->values[i] == 0.0f)
                        continue;

                printf(
                        "  %-20s %.4f\n",
                        data->vocabulary.entries[i].word,
                        vector->values[i]
                );
        }
}

static size_t tokenVocabularyIndex(const ClassifierData* data, const char* token) {
	return vocabularyFind(&data->vocabulary, token);
}

static bool textToVector(const ClassifierData* data, const char* text, Vector* vector) {
	vector->values = NULL;
	vector->size = 0;

	if (data->vocabulary.count == 0) return true;

	vector->values = calloc(data->vocabulary.count, sizeof(float));

	if (vector->values == NULL) return false;

	vector->size = data->vocabulary.count;

	TokenList tokens = tokenize(text);

	if (tokens.count == 0) {
		tokenListDestroy(&tokens);
		return true;
	}

	size_t* frequencies = calloc(data->vocabulary.count, sizeof(size_t));

	if (frequencies == NULL) {
		tokenListDestroy(&tokens);
		vectorDestroy(vector);
		return false;
	}

	for (size_t i = 0; i < tokens.count; i++) {
		size_t index = tokenVocabularyIndex(data, tokens.tokens[i]);

		if (index != SIZE_MAX) frequencies[index]++;
	}

	for (size_t i = 0; i < data->vocabulary.count; i++) {
		if (frequencies[i] == 0) continue;

		float tf = (float)frequencies[i]/(float)tokens.count;

		vector->values[i] = tf * data->idf[i];
	}

	free(frequencies);
	tokenListDestroy(&tokens);

	return true;
}

static void commandVectorDestroy(CommandVector* command) {
	if (command == NULL) return;

	for (size_t i = 0; i < command->exampleCount; i++) {
		vectorDestroy(&command->examples[i].vector);
	}

	free(command->examples);
	command->examples = NULL;
	command->exampleCount = 0;
	command->command = NULL;
}

static bool buildCommandVector(const ClassifierData* data, const CommandDefinition* command, CommandVector* result) {
	result->command = command;
	result->examples = NULL;
	result->exampleCount = 0;

	if (command->exampleCount == 0) return true;

	result->examples = calloc(command->exampleCount, sizeof(ExampleVector));

	if (result->examples == NULL) return false;

	result->exampleCount = command->exampleCount;

	for (size_t i = 0; i < command->exampleCount; i++) {
		if (!textToVector(data, command->examples[i], &result->examples[i].vector)) {
			commandVectorDestroy(result);
			return false;
		}
	}

	return true;
}

static bool buildCommandVectors(ClassifierData* data, const CommandRegistry* registry) {
	data->commandCount = commandRegistryCount(registry);

	if (data->commandCount == 0) return true;

	data->commands = calloc(data->commandCount, sizeof(CommandVector));

	if (data->commands == NULL) return false;

	for (size_t i = 0; i < data->commandCount; i++) {
		const CommandDefinition* command = commandRegistryGet(registry, i);

		if (command == NULL) continue;

		if (!buildCommandVector(data, command, &data->commands[i])) return false;
	}

	return true;
}

static float cosineSimilarity(const float* a, const float* b, size_t size) {
	float dot = 0.0f;
	float magnitudeA = 0.0f;
	float magnitudeB = 0.0f;

	for (size_t i = 0; i < size; i++) {
		dot += a[i] * b[i];
		magnitudeA += a[i] * a[i];
		magnitudeB += b[i] * b[i];
	}

	if (magnitudeA == 0.0f || magnitudeB == 0.0f) return 0.0f;

	return dot/(sqrtf(magnitudeA)*sqrtf(magnitudeB));
}

static float commandSimilarity(const CommandVector* command, const Vector* request) {
	float bestScore = 0.0f;

	for (size_t i = 0; i < command->exampleCount; i++) {
		const Vector* example = &command->examples[i].vector;

		if (example->size != request->size) continue;

		float score = cosineSimilarity(request->values, example->values, request->size);

		if (score > bestScore) bestScore = score;
	}

	return bestScore;
}

static float keywordScore(const CommandDefinition* command, const char* text) {
	if (command == NULL || command->keywordCount == 0 || text == NULL) return 0.0f;

	TokenList tokens = tokenize(text);

	if (tokens.count == 0) {
		tokenListDestroy(&tokens);
		return 0.0f;
	}

	size_t matches = 0;

	for (size_t i = 0; i < command->keywordCount; i++) {
		const char* keyword = command->keywords[i];

		for (size_t j = 0; j < tokens.count; j++) {
			if (strcmp(keyword, tokens.tokens[j]) == 0) {
				matches++;
				break;
			}
		}
	}

	tokenListDestroy(&tokens);

	return (float)matches/(float)command->keywordCount;
}

static void addCandidate(ClassificationResult* result, const CommandDefinition* command, float score) {
	size_t position = result->count;

	for (size_t i = 0; i < result->count; i++) {
		if (score > result->candidates[i].score) {
			position = i;
			break;
		}
	}

	if (result->count < CLASSIFIER_MAX_CANDIDATES) {
		for (size_t i = result->count; i > position; i--) {
			result->candidates[i] = result->candidates[i - 1];
		}

		result->candidates[position] = (ClassificationCandidate) { .command = command, .score = score };

		result->count++;
		return;
	}

	if (position >= CLASSIFIER_MAX_CANDIDATES) return;

	for (size_t i = CLASSIFIER_MAX_CANDIDATES - 1; i > position; i--) {
		result->candidates[i] = result->candidates[i - 1];
	}

	result->candidates[position] = (ClassificationCandidate) { .command = command, .score = score };
}

static void classifierDataDestroy(ClassifierData* data) {
	if (data == NULL) return;

	vocabularyDestroy(&data->vocabulary);

	free(data->idf);
	data->idf = NULL;

	for (size_t i = 0; i < data->commandCount; i++) {
		commandVectorDestroy(&data->commands[i]);
	}
	free(data->commands);

	data->commands = NULL;
	data->commandCount = 0;

	free(data);
}

bool classifierInit(Classifier* classifier, const CommandRegistry* registry) {
	if (classifier == NULL || registry == NULL) return false;

	classifier->data = NULL;

	ClassifierData* data = calloc(1, sizeof(ClassifierData));
	if (data == NULL) return false;

	vocabularyInit(&data->vocabulary);

	if (!buildVocabulary(data, registry)) {
		classifierDataDestroy(data);
		return false;
	}

	if (!calculateIDF(data)) {
		classifierDataDestroy(data);
		return false;
	}

	if (!buildCommandVectors(data, registry)) {
		classifierDataDestroy(data);
		return false;
	}

	classifier->data = data;
	return true;
}

void classifierDestroy(Classifier* classifier) {
	if (classifier == NULL) return;

	ClassifierData* data = classifier->data;

	if (data != NULL) classifierDataDestroy(data);

	classifier->data = NULL;
}

ClassificationResult classifierClassify(Classifier* classifier, const char* text) {
	ClassificationResult result = { .count = 0 };

	if (classifier == NULL || classifier->data == NULL || text == NULL) return result;

	ClassifierData* data = classifier->data;

	Vector requestVector;

	if (!textToVector(data, text, &requestVector)) return result;

	for (size_t i = 0; i < data->commandCount; i++) {
		CommandVector* command = &data->commands[i];

		if (command->command == NULL) continue;

		float tfidfScore = commandSimilarity(command, &requestVector);
		float keywords = keywordScore(command->command, text);

		float score = (tfidfScore * 0.8f) + (keywords * 0.2f);

		addCandidate(&result, command->command, score);
	}

	vectorDestroy(&requestVector);

	return result;
}

bool classifierIsConfident(const ClassificationResult* result) {
	if (result == NULL || result->count == 0) return false;

	float best = result->candidates[0].score;

	if (best < MIN_CONFIDENCE) return false;

	if (result->count >= 2) {
		float second = result->candidates[1].score;

		if ((best - second) < MIN_SCORE_MARGIN) return false;
	}

	return true;
}
