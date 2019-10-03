/*
 * Copyright (c) 2019 Actinius
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "accelerometer.h"

#include <stdio.h>

int main(void)
{
	if (!init_accelerometer()) {
		return -1;
	}

	while (1) {
		printk("\033[1;1H");
		printk("\033[2J");

		double x_accel = 0;
		double y_accel = 0;
		double z_accel = 0;
		get_accelerometer_data(&x_accel, &y_accel, &z_accel);

		printk("Acceleration values:\n");
		printk("------------------------------------------------------------------------------------------------------\n");
		printf("X acceleration: %lf (m/s^2), Y acceleration: %lf (m/s^2), Z acceleration: %lf (m/s^2)\n", x_accel, y_accel, z_accel);
		printk("------------------------------------------------------------------------------------------------------");

		k_sleep(K_MSEC(500));
	}

	return 0;
}

