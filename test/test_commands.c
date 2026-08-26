#include "command.h"

/*
 * ============================================================
 * SYSTEM COMMANDS
 * ============================================================
 */

static const char* timeExamples[] = {
        "what time is it",
        "what's the time",
        "tell me the current time",
        "what is the current time",
        "give me the time",
        "do you know what time it is"
};

static const char* timeKeywords[] = {
        "time",
        "clock",
        "current"
};

const CommandDefinition commandGetTime = {
        .id = "system.get_time",
        .name = "Get Time",
        .description = "Gets the current local time.",

        .examples = timeExamples,
        .exampleCount = sizeof(timeExamples) /
                        sizeof(timeExamples[0]),

        .keywords = timeKeywords,
        .keywordCount = sizeof(timeKeywords) /
                        sizeof(timeKeywords[0]),

        .parameters = NULL,
        .parameterCount = 0,

        .requiresConfirmation = false
};


static const char* dateExamples[] = {
        "what day is it",
        "what is today's date",
        "tell me today's date",
        "what date is it",
        "what day are we on",
        "give me the date"
};

static const char* dateKeywords[] = {
        "date",
        "day",
        "today"
};

const CommandDefinition commandGetDate = {
        .id = "system.get_date",
        .name = "Get Date",
        .description = "Gets the current date.",

        .examples = dateExamples,
        .exampleCount = sizeof(dateExamples) /
                        sizeof(dateExamples[0]),

        .keywords = dateKeywords,
        .keywordCount = sizeof(dateKeywords) /
                        sizeof(dateKeywords[0]),

        .parameters = NULL,
        .parameterCount = 0,

        .requiresConfirmation = false
};


static const char* cpuTemperatureExamples[] = {
        "what is my cpu temperature",
        "how hot is my processor",
        "check my cpu temperature",
        "what temperature is my processor running at",
        "is my cpu overheating",
        "tell me how hot my processor is",
        "check how hot the cpu is"
};

static const char* cpuTemperatureKeywords[] = {
        "cpu",
        "processor",
        "temperature",
        "hot",
        "overheating"
};

const CommandDefinition commandGetCpuTemperature = {
        .id = "system.get_cpu_temperature",
        .name = "Get CPU Temperature",
        .description = "Gets the current CPU temperature.",

        .examples = cpuTemperatureExamples,
        .exampleCount = sizeof(cpuTemperatureExamples) /
                        sizeof(cpuTemperatureExamples[0]),

        .keywords = cpuTemperatureKeywords,
        .keywordCount = sizeof(cpuTemperatureKeywords) /
                        sizeof(cpuTemperatureKeywords[0]),

        .parameters = NULL,
        .parameterCount = 0,

        .requiresConfirmation = false
};


static const char* memoryExamples[] = {
        "how much memory am i using",
        "how much ram is being used",
        "check my memory usage",
        "what is my ram usage",
        "how much memory do i have left",
        "tell me how much ram is available",
        "check how much memory is available"
};

static const char* memoryKeywords[] = {
        "memory",
        "ram",
        "usage",
        "available"
};

const CommandDefinition commandGetMemory = {
        .id = "system.get_memory",
        .name = "Get Memory Usage",
        .description = "Gets current memory usage.",

        .examples = memoryExamples,
        .exampleCount = sizeof(memoryExamples) /
                        sizeof(memoryExamples[0]),

        .keywords = memoryKeywords,
        .keywordCount = sizeof(memoryKeywords) /
                        sizeof(memoryKeywords[0]),

        .parameters = NULL,
        .parameterCount = 0,

        .requiresConfirmation = false
};


/*
 * ============================================================
 * TIMER COMMANDS
 * ============================================================
 */

static const char* timerExamples[] = {
        "set a timer for five minutes",
        "set a five minute timer",
        "start a timer for ten minutes",
        "set a timer",
        "start a timer",
        "create a timer for fifteen minutes",
        "remind me in twenty minutes"
};

static const char* timerKeywords[] = {
        "timer",
        "minutes",
        "seconds",
        "hours",
        "remind"
};

static const CommandParameter timerParameters[] = {
        {
                .name = "duration",
                .type = PARAM_DURATION,
                .required = true,

                .enumValues = NULL,
                .enumValueCount = 0
        }
};

const CommandDefinition commandSetTimer = {
        .id = "system.set_timer",
        .name = "Set Timer",
        .description = "Creates a timer with a specified duration.",

        .examples = timerExamples,
        .exampleCount = sizeof(timerExamples) /
                        sizeof(timerExamples[0]),

        .keywords = timerKeywords,
        .keywordCount = sizeof(timerKeywords) /
                        sizeof(timerKeywords[0]),

        .parameters = timerParameters,
        .parameterCount = 1,

        .requiresConfirmation = false
};


/*
 * ============================================================
 * APPLICATION COMMANDS
 * ============================================================
 */

static const char* launchExamples[] = {
        "open firefox",
        "launch firefox",
        "start firefox",
        "open my browser",
        "launch the browser",
        "open the terminal",
        "start the terminal"
};

static const char* launchKeywords[] = {
        "open",
        "launch",
        "start",
        "run"
};

const CommandDefinition commandLaunchApplication = {
        .id = "application.launch",
        .name = "Launch Application",
        .description = "Launches an application.",

        .examples = launchExamples,
        .exampleCount = sizeof(launchExamples) /
                        sizeof(launchExamples[0]),

        .keywords = launchKeywords,
        .keywordCount = sizeof(launchKeywords) /
                        sizeof(launchKeywords[0]),

        .parameters = NULL,
        .parameterCount = 0,

        .requiresConfirmation = false
};


/*
 * ============================================================
 * COMMAND LIST
 *
 * This makes it easy for main.c to register every test
 * command without knowing about the individual definitions.
 * ============================================================
 */

const CommandDefinition* testCommands[] = {
        &commandGetTime,
        &commandGetDate,
        &commandGetCpuTemperature,
        &commandGetMemory,
        &commandSetTimer,
        &commandLaunchApplication
};

const size_t testCommandCount =
        sizeof(testCommands) /
        sizeof(testCommands[0]);
