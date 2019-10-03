/*
 * Copyright (c) 2019 Actinius
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include <zephyr.h>

bool init_accelerometer(void);
void get_accelerometer_data(double *x_accel, double *y_accel, double *z_accel);

#endif // _ACCELEROMETER_H_
