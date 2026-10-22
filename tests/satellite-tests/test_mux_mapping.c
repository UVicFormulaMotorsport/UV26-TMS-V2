#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "mux_mapping.h"

typedef struct
{
    uint8_t mux;
    uint8_t channel;
    uint8_t expected_index;
} SensorMapping;

static unsigned int tests_run = 0;
static unsigned int failures = 0;

static void report_result(bool passed, const char *description)
{
    ++tests_run;
    if (!passed)
    {
        ++failures;
    }
    printf("[%s] %s\n", passed ? "PASS" : "FAIL", description);
}

static void test_valid_sensor_mappings(void)
{
    static const SensorMapping mappings[] = {
        {0, 0, 2}, {0, 1, 1}, {0, 2, 0}, {0, 3, 3},
        {0, 4, 6}, {0, 5, 5}, {0, 6, 4}, {0, 7, 7},
        {1, 0, 8}, {1, 1, 9}, {1, 2, 10}, {1, 3, 11},
        {1, 4, 12}, {1, 5, 13}, {1, 6, 14}, {1, 7, 15},
        {2, 0, 17}, {2, 1, 18}, {2, 2, 19}, {2, 3, 16},
        {2, 4, 20}, {2, 5, 21}, {2, 6, 22}
    };
    bool seen_indices[23] = {false};
    bool mappings_valid = true;

    _Static_assert(sizeof(mappings) / sizeof(mappings[0]) == 23,
                   "all sensor mappings must be covered");

    for (size_t i = 0; i < sizeof(mappings) / sizeof(mappings[0]); ++i)
    {
        const SensorMapping *mapping = &mappings[i];
        uint8_t actual_index = UINT8_MAX;
        const int status = mux_get_sensor_index(mapping->mux,
                                                mapping->channel,
                                                &actual_index);
        const bool in_range = actual_index < sizeof(seen_indices) / sizeof(seen_indices[0]);
        const bool unique = in_range && !seen_indices[actual_index];
        const bool passed = status == MUX_MAPPING_OK &&
                            actual_index == mapping->expected_index &&
                            unique;
        char description[128];
        snprintf(description, sizeof(description),
                 "MUX %u, channel %u -> sensor %u (actual %u, status %d)",
                 (unsigned)mapping->mux, (unsigned)mapping->channel,
                 (unsigned)mapping->expected_index, (unsigned)actual_index, status);
        report_result(passed, description);
        mappings_valid = mappings_valid && passed;
        if (in_range)
        {
            seen_indices[actual_index] = true;
        }
    }

    bool complete = mappings_valid;
    for (size_t i = 0; i < sizeof(seen_indices) / sizeof(seen_indices[0]); ++i)
    {
        complete = complete && seen_indices[i];
    }
    report_result(complete, "all 23 sensor indices mapped exactly once");
}

static void test_unused_mux03_channel_7(void)
{
    uint8_t sensor_index = UINT8_MAX;
    const int status = mux_get_sensor_index(2, 7, &sensor_index);
    report_result(status == MUX_MAPPING_UNUSED_CHANNEL && sensor_index == UINT8_MAX,
                  "MUX 2, channel 7 is unused; output unchanged");
}

static void check_invalid(uint8_t mux, uint8_t channel, const char *description)
{
    uint8_t sensor_index = UINT8_MAX;
    const int status = mux_get_sensor_index(mux, channel, &sensor_index);
    report_result(status == MUX_MAPPING_INVALID_ARGUMENT && sensor_index == UINT8_MAX,
                  description);
}

static void test_invalid_arguments(void)
{
    check_invalid(3, 0, "reject MUX index 3; output unchanged");
    check_invalid(UINT8_MAX, 0, "reject MUX index UINT8_MAX; output unchanged");
    check_invalid(0, 8, "reject channel 8; output unchanged");
    check_invalid(0, UINT8_MAX, "reject channel UINT8_MAX; output unchanged");
    report_result(mux_get_sensor_index(0, 0, NULL) == MUX_MAPPING_INVALID_ARGUMENT,
                  "reject NULL output pointer");
}

int main(void)
{
    puts("Satellite MUX mapping test suite");
    test_valid_sensor_mappings();
    test_unused_mux03_channel_7();
    test_invalid_arguments();
    printf("\n%u tests, %u failures\n", tests_run, failures);
    return failures == 0 ? 0 : 1;
}
