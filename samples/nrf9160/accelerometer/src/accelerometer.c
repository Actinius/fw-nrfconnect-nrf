/*
 * Copyright (c) 2019 Actinius
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "accelerometer.h"

#include <sensor.h>

struct device *accel;

bool init_accelerometer(void)
{
	accel = device_get_binding("LIS2DH12-ACCEL");

	if (!accel) {
		printk("Error getting accelerometer device binding\r\n");

		return false;
	}

	return true;
}

void get_accelerometer_data(double *x_accel, double *y_accel, double *z_accel)
{
	sensor_sample_fetch(accel);

	struct sensor_value value_x;
	sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &value_x);
	*x_accel = sensor_value_to_double(&value_x);

	struct sensor_value value_y;
	sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Y, &value_y);
	*y_accel = sensor_value_to_double(&value_y);

	struct sensor_value value_z;
	sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Z, &value_z);
	*z_accel = sensor_value_to_double(&value_z);
}