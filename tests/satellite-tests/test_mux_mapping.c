#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "mux_mapping.h"

typedef struct
{
    uint8_t mux;
    uint8_t channel;
    uint8_t expected_index;
} SensorMapping;

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

    _Static_assert(sizeof(mappings) / sizeof(mappings[0]) == 23,
                   "all sensor mappings must be covered");

    for (uint8_t mapping_index = 0;
         mapping_index < sizeof(mappings) / sizeof(mappings[0]);
         ++mapping_index)
    {
        uint8_t sensor_index = UINT8_MAX;
        const SensorMapping *mapping = &mappings[mapping_index];

        assert(mux_get_sensor_index(
                   mapping->mux, mapping->channel, &sensor_index) ==
               MUX_MAPPING_OK);
        assert(sensor_index == mapping->expected_index);
        assert(sensor_index < sizeof(seen_indices) / sizeof(seen_indices[0]));
        assert(!seen_indices[sensor_index]);

        seen_indices[sensor_index] = true;
    }

    for (uint8_t sensor_index = 0;
         sensor_index < sizeof(seen_indices) / sizeof(seen_indices[0]);
         ++sensor_index)
    {
        assert(seen_indices[sensor_index]);
    }
}

static void test_unused_mux03_channel_7(void)
{
    uint8_t sensor_index = UINT8_MAX;

    assert(mux_get_sensor_index(2, 7, &sensor_index) ==
           MUX_MAPPING_UNUSED_CHANNEL);
    assert(sensor_index == UINT8_MAX);
}

static void test_invalid_arguments(void)
{
    uint8_t sensor_index = UINT8_MAX;

    assert(mux_get_sensor_index(3, 0, &sensor_index) ==
           MUX_MAPPING_INVALID_ARGUMENT);
    assert(sensor_index == UINT8_MAX);

    assert(mux_get_sensor_index(UINT8_MAX, 0, &sensor_index) ==
           MUX_MAPPING_INVALID_ARGUMENT);
    assert(sensor_index == UINT8_MAX);

    assert(mux_get_sensor_index(0, 8, &sensor_index) ==
           MUX_MAPPING_INVALID_ARGUMENT);
    assert(sensor_index == UINT8_MAX);

    assert(mux_get_sensor_index(0, UINT8_MAX, &sensor_index) ==
           MUX_MAPPING_INVALID_ARGUMENT);
    assert(sensor_index == UINT8_MAX);

    assert(mux_get_sensor_index(0, 0, NULL) == MUX_MAPPING_INVALID_ARGUMENT);
}

int main(void)
{
    test_valid_sensor_mappings();
    test_unused_mux03_channel_7();
    test_invalid_arguments();

    return 0;
}
