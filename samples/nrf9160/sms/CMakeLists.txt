#
# Copyright (c) 2019 Actinius
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.8.2)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(sms_sample)

zephyr_include_directories(src)
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/pdu.c)
