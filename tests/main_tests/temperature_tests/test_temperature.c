#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "temperature.h"

static unsigned tests_run;
static unsigned tests_failed;

static void report_result(const char *name, bool passed, const char *detail)
{
	tests_run++;
	if (!passed)
		tests_failed++;

	printf("[%s] %s", passed ? "PASS" : "FAIL", name);
	if (detail != NULL)
		printf(": %s", detail);
	putchar('\n');
}

static bool close_enough(float actual, float expected, float tolerance)
{
	return isfinite(actual) && fabsf(actual - expected) <= tolerance;
}

static void test_lut_anchors(void)
{
	static const struct
	{
		float voltage;
		float expected_c;
	} anchors[] = {
		{2.44f, -40.0f}, {2.42f, -35.0f}, {2.40f, -30.0f},
		{2.38f, -25.0f}, {2.35f, -20.0f}, {2.32f, -15.0f},
		{2.27f, -10.0f}, {2.23f, -5.0f}, {2.17f, 0.0f},
		{2.11f, 5.0f}, {2.05f, 10.0f}, {1.99f, 15.0f},
		{1.92f, 20.0f}, {1.86f, 25.0f}, {1.80f, 30.0f},
		{1.74f, 35.0f}, {1.68f, 40.0f}, {1.63f, 45.0f},
		{1.59f, 50.0f}, {1.55f, 55.0f}, {1.51f, 60.0f},
		{1.48f, 65.0f}, {1.45f, 70.0f}, {1.43f, 75.0f},
		{1.40f, 80.0f}, {1.38f, 85.0f}, {1.37f, 90.0f},
		{1.35f, 95.0f}, {1.34f, 100.0f}, {1.33f, 105.0f},
		{1.32f, 110.0f}, {1.31f, 115.0f}, {1.30f, 120.0f},
	};

	for (size_t i = 0; i < sizeof(anchors) / sizeof(anchors[0]); ++i)
	{
		float actual_c = NAN;
		TemperatureStatus status = temperature_voltage_to_celsius(anchors[i].voltage, &actual_c);
		char name[64];
		char detail[96];
		(void)snprintf(name, sizeof(name), "LUT anchor %u (%0.2f V)", (unsigned)i, anchors[i].voltage);
		(void)snprintf(detail, sizeof(detail), "expected %0.3f C, got %0.3f C (status %d)", anchors[i].expected_c, actual_c, (int)status);
		report_result(name, status == TEMPERATURE_STATUS_OK && close_enough(actual_c, anchors[i].expected_c, 0.001f), detail);
	}
}

static void test_known_interpolation(void)
{
	static const struct
	{
		float voltage;
		float expected_c;
	} samples[] = {
		{2.43f, -37.5f},
		{2.365f, -22.5f},
		{1.83f, 27.5f},
		{1.39f, 82.5f},
	};

	for (size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i)
	{
		float actual_c = NAN;
		TemperatureStatus status = temperature_voltage_to_celsius(samples[i].voltage, &actual_c);
		char name[64];
		char detail[96];
		(void)snprintf(name, sizeof(name), "interpolation sample %u (%0.2f V)", (unsigned)i, samples[i].voltage);
		(void)snprintf(detail, sizeof(detail), "expected %0.3f C, got %0.3f C (status %d)", samples[i].expected_c, actual_c, (int)status);
		report_result(name, status == TEMPERATURE_STATUS_OK && close_enough(actual_c, samples[i].expected_c, 0.001f), detail);
	}
}

static void test_monotonic_conversion(void)
{
	bool passed = true;
	unsigned failed_sample = 0;
	float failed_voltage = 0.0f;
	float failed_temperature = 0.0f;
	float previous_c = -INFINITY;
	for (unsigned i = 0; i <= 1000; ++i)
	{
		float voltage = 2.44f - (1.14f * (float)i / 1000.0f);
		float current_c = NAN;
		TemperatureStatus status = temperature_voltage_to_celsius(voltage, &current_c);
		if (status != TEMPERATURE_STATUS_OK || !isfinite(current_c) || current_c < previous_c)
		{
			passed = false;
			failed_sample = i;
			failed_voltage = voltage;
			failed_temperature = current_c;
			break;
		}
		previous_c = current_c;
	}
	char detail[112];
	if (passed)
		(void)snprintf(detail, sizeof(detail), "1001 samples from 2.44 V to 1.30 V");
	else
		(void)snprintf(detail, sizeof(detail), "sample %u at %0.4f V produced %0.3f C", failed_sample, failed_voltage, failed_temperature);
	report_result("dense voltage sweep is finite and monotonic", passed, detail);
}

static void test_invalid_voltages(void)
{
	static const struct
	{
		const char *name;
		float voltage;
	} invalid[] = {
		{"above maximum", 2.4401f},
		{"below minimum", 1.2999f},
		{"zero", 0.0f},
		{"negative", -1.0f},
		{"positive infinity", INFINITY},
		{"negative infinity", -INFINITY},
		{"NaN", NAN},
	};

	for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
	{
		float output = 1234.5f;
		TemperatureStatus status = temperature_voltage_to_celsius(invalid[i].voltage, &output);
		char name[80];
		(void)snprintf(name, sizeof(name), "reject %s", invalid[i].name);
		report_result(name, status == TEMPERATURE_STATUS_INVALID_VOLTAGE && output == 1234.5f, "output remains unchanged");
	}
}

int main(void)
{
	puts("Temperature conversion test suite");
	test_lut_anchors();
	test_known_interpolation();
	test_monotonic_conversion();
	test_invalid_voltages();
	printf("\n%u tests, %u failures\n", tests_run, tests_failed);
	return tests_failed == 0 ? 0 : 1;
}
