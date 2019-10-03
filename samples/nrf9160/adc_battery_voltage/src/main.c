/*
 * Copyright (c) 2019 Actinius
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <adc.h>
#include <hal/nrf_saadc.h>

#define BATVOLT_R1 4.7f                 // MOhm
#define BATVOLT_R2 10.0f                // MOhm
#define INPUT_VOLT_RANGE 3.6f           // Volts
#define VALUE_RANGE_10_BIT 1.023        // (2^10 - 1) / 1000

#define ADC_DEV "ADC_0"

#define ADC_RESOLUTION 10
#define ADC_GAIN ADC_GAIN_1_6
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)
#define ADC_1ST_CHANNEL_ID 0
#define ADC_1ST_CHANNEL_INPUT NRF_SAADC_INPUT_AIN0

#define BUFFER_SIZE 1
static s16_t m_sample_buffer[BUFFER_SIZE];

static struct device *adc_dev;

static const struct adc_channel_cfg m_1st_channel_cfg = {
	.gain = ADC_GAIN,
	.reference = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id = ADC_1ST_CHANNEL_ID,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
	.input_positive = ADC_1ST_CHANNEL_INPUT,
#endif
};

static int get_battery_voltage(u16_t *battery_voltage)
{
	int err;

	const struct adc_sequence sequence = {
		.channels = BIT(ADC_1ST_CHANNEL_ID),
		.buffer = m_sample_buffer,
		.buffer_size = sizeof(m_sample_buffer), // in bytes!
		.resolution = ADC_RESOLUTION,
	};

	if (!adc_dev) {
		return -1;
	}

	err = adc_read(adc_dev, &sequence);
	if (err) {
		printk("ADC read err: %d\n", err);

		return err;
	}

	float sample_value = 0;
	for (int i = 0; i < BUFFER_SIZE; i++) {
		sample_value += (float) m_sample_buffer[i];
	}
	sample_value /= BUFFER_SIZE;

	*battery_voltage = (u16_t)(sample_value * (INPUT_VOLT_RANGE / VALUE_RANGE_10_BIT) * ((BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2));

	return 0;
}

bool init_adc()
{
	int err;

	adc_dev = device_get_binding(ADC_DEV);
	if (!adc_dev) {
		printk("Error getting " ADC_DEV " failed\n");

		return false;
	}

	err = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
	if (err) {
		printk("Error in adc setup: %d\n", err);

		return false;
	}

	// Trigger offset calibration
	// As this generates a _DONE and _RESULT event
	// the first result will be incorrect.
	NRF_SAADC_NS->TASKS_CALIBRATEOFFSET = 1;

	// discard incorrect sample
	u16_t dummy = 0;
	get_battery_voltage(&dummy);

	return true;
}

int main(void)
{
	init_adc();

	while (1) {
		u16_t battery_voltage = 0;
		get_battery_voltage(&battery_voltage);
		printk("Battery voltage: %u mV\n", battery_voltage);

		k_sleep(K_MSEC(2000));
	}

	return 0;
}


