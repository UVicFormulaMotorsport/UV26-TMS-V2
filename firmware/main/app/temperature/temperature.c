#include "temperature.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
	float voltage;
	float temperature_c;
} TemperatureLutEntry;

static const TemperatureLutEntry temperature_lut[] =
	{
		{2.44f, -40.0f},
		{2.42f, -35.0f},
		{2.40f, -30.0f},
		{2.38f, -25.0f},
		{2.35f, -20.0f},
		{2.32f, -15.0f},
		{2.27f, -10.0f},
		{2.23f, -5.0f},
		{2.17f, 0.0f},
		{2.11f, 5.0f},
		{2.05f, 10.0f},
		{1.99f, 15.0f},
		{1.92f, 20.0f},
		{1.86f, 25.0f},
		{1.80f, 30.0f},
		{1.74f, 35.0f},
		{1.68f, 40.0f},
		{1.63f, 45.0f},
		{1.59f, 50.0f},
		{1.55f, 55.0f},
		{1.51f, 60.0f},
		{1.48f, 65.0f},
		{1.45f, 70.0f},
		{1.43f, 75.0f},
		{1.40f, 80.0f},
		{1.38f, 85.0f},
		{1.37f, 90.0f},
		{1.35f, 95.0f},
		{1.34f, 100.0f},
		{1.33f, 105.0f},
		{1.32f, 110.0f},
		{1.31f, 115.0f},
		{1.30f, 120.0f}};

float temperature_from_adc(uint16_t adc_value)
{
	/*
	 * Converts a raw ADC measurement received from a TMS satellite
	 * into temperature in degrees Celsius.
	 *
	 * Conversion path:
	 * raw ADC -> sensor voltage -> LUT interpolation -> deg C
	 *
	 * TODO: Confirm ADC-to-sensor-voltage conversion with electrical team.
	 */
}

static bool find_lut_interval(float voltage, size_t *first_lut_index)
{
	// a binary search implementation to find the interval which the target voltage is betweeen.
	// we then interpolate between these values

	size_t lut_size = sizeof(temperature_lut) / sizeof(temperature_lut[0]);

	size_t table_low = 0;
	size_t table_high = lut_size - 2;
	// use size_t when addressing an array

	// check if search range is valid
	if (voltage > temperature_lut[table_low].voltage || voltage < temperature_lut[table_high + 1].voltage)
	{
		// invalid search range
		return false;
	}
	// search is within valid range
	// binary search implementation
	while (table_low <= table_high)
	{
		size_t table_mid = table_low + ((table_high - table_low) / 2);

		if (temperature_lut[table_mid].voltage >= voltage && temperature_lut[table_mid + 1].voltage <= voltage)
		{
			// we're at the correct interval
			*first_lut_index = table_mid; // set table mid at the address of lower_lut_index
			// now table mid is the first number of the interval in which the voltage is between
			return true;
		}
		else if (voltage > temperature_lut[table_mid].voltage)
		{
			table_high = table_mid - 1;
		}
		else
		{
			table_low = table_mid + 1;
		}
	}
	return false;
}

TemperatureStatus temperature_voltage_to_celsius(float voltage, float *temperature_c)
{
	size_t first_lut_index;

	// TODO: Enepaq LUT lookup + linear interpolation

	// 1. Ask find_lut_interval() for the interval

	if (find_lut_interval(voltage, &first_lut_index) == false)
	{
		return TEMPERATURE_STATUS_INVALID_VOLTAGE;
	}

	// 2. Get the two LUT entries as it is valid
	TemperatureLutEntry first_entry = temperature_lut[first_lut_index];
	TemperatureLutEntry second_entry = temperature_lut[first_lut_index + 1];

	// 3. Interpolate
	float fraction = (voltage - first_entry.voltage) / (second_entry.voltage - first_entry.voltage);
	*temperature_c = first_entry.temperature_c + (fraction * (second_entry.temperature_c - first_entry.temperature_c));
	// 4. Return °C
	return TEMPERATURE_STATUS_OK;
}
//test 
