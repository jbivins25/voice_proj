#include "assistant.h"
#include "formatter.h"
#include "llm.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const CommandDefinition* testCommands[];
extern const size_t testCommandCount;

extern const ToolDefinition* testTools[];
extern const size_t testToolCount;

int main(void)
{
Assistant assistant;

    if (!assistantInit(
            &assistant,
            testCommands,
            testCommandCount,
            testTools,
            testToolCount)) {

            fprintf(
                    stderr,
                    "Failed to initialize assistant.\n"
            );

            return EXIT_FAILURE;
    }

    if (!llmInit()) {
            fprintf(
                    stderr,
                    "Failed to initialize LLM.\n"
            );

            assistantDestroy(
                    &assistant
            );

            return EXIT_FAILURE;
    }

    printf(
            "Registered %zu commands.\n",
            testCommandCount
    );

    printf(
            "Registered %zu tools.\n",
            testToolCount
    );

    printf(
            "\n"
            "Assistant initialized.\n"
            "Type a request.\n"
            "Type 'quit' to exit.\n\n"
    );

    char input[1024];
    unsigned long convId = 1;

    while (true) {

            printf("> ");
            fflush(stdout);

            if (fgets(
                    input,
                    sizeof(input),
                    stdin
            ) == NULL)
                    break;

            input[
                    strcspn(
                            input,
                            "\r\n"
                    )
            ] = '\0';

            if (strcmp(input, "quit") == 0 ||
                strcmp(input, "exit") == 0)
                    break;

            if (input[0] == '\0')
                    continue;

            Response response =
                    assistantProcess(
                            &assistant,
                            input,
                            convId
                    );

            FormattedResponse formatted =
                    formatterFormat(
                            &response
                    );

            if (formatted.display &&
                formatted.text != NULL) {

                    printf(
                            "%s\n",
                            formatted.text
                    );
            }

            formatterDestroy(
                    &formatted
            );

            responseDestroy(
                    &response
            );
    }

    llmDestroy();

    assistantDestroy(
            &assistant
    );

    return EXIT_SUCCESS;
}

