#
# Copyright (c) 2021 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(debug_module_test)

# Set CMake path variables for convenience
set(MEMFAULT_SDK_DIR ${ZEPHYR_BASE}/../modules/lib/memfault-firmware-sdk)
set(NRF_SDK_DIR ${ZEPHYR_BASE}/../nrf)
set(ASSET_TRACKER_V2_DIR ../..)

# Generate runner for the test
test_runner_generate(src/debug_module_test.c)

# Create mock for Memfault SDK source files
cmock_handle(${MEMFAULT_SDK_DIR}/components/include/memfault/metrics/metrics.h memfault/metrics)
cmock_handle(${MEMFAULT_SDK_DIR}/ports/zephyr/include/memfault/ports/zephyr/http.h memfault/ports/zephyr)
cmock_handle(${MEMFAULT_SDK_DIR}/components/include/memfault/core/data_packetizer.h memfault/core)
cmock_handle(${MEMFAULT_SDK_DIR}/components/include/memfault/core/trace_event.h memfault/core)
cmock_handle(${MEMFAULT_SDK_DIR}/ports/include/memfault/ports/watchdog.h memfault/ports)

# Create mock for nRF SDK
cmock_handle(${NRF_SDK_DIR}/include/event_manager.h)
cmock_handle(${ZEPHYR_BASE}/include/sys/reboot.h)

# Create mock for Asset Tracker v2 application source files
cmock_handle(${ASSET_TRACKER_V2_DIR}/src/modules/modules_common.h)
cmock_handle(${ASSET_TRACKER_V2_DIR}/src/watchdog/watchdog_app.h)

# Add debug module (Unit Under Test)
target_sources(app PRIVATE ${ASSET_TRACKER_V2_DIR}/src/modules/debug_module.c)

# Add test source file
target_sources(app PRIVATE src/debug_module_test.c)

# Include Asset Tracker v2 application folders
target_include_directories(app PRIVATE .)
target_include_directories(app PRIVATE ${ASSET_TRACKER_V2_DIR}/src/)
target_include_directories(app PRIVATE ${ASSET_TRACKER_V2_DIR}/src/watchdog)
target_include_directories(app PRIVATE ${ASSET_TRACKER_V2_DIR}/src/modules/)
target_include_directories(app PRIVATE ${ASSET_TRACKER_V2_DIR}/src/events/)
target_include_directories(app PRIVATE ${ASSET_TRACKER_V2_DIR}/configuration/memfault/)

# Include nRF SDK directories
target_include_directories(app PRIVATE ${NRF_SDK_DIR}/subsys/event_manager)
target_include_directories(app PRIVATE ${NRF_SDK_DIR}/modules/cjson/include)

# Include Memfault SDK directories
target_include_directories(app PRIVATE ${MEMFAULT_SDK_DIR}/components/include/)
target_include_directories(app PRIVATE ${MEMFAULT_SDK_DIR}/components/include/core)

# Include nRFxlib directories
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/../nrfxlib/nrf_modem/include/)

# Options that cannot be passed through Kconfig fragments.
target_compile_options(app PRIVATE
  	-DCONFIG_ASSET_TRACKER_V2_APP_VERSION_MAX_LEN=20
	-DCONFIG_DEBUG_MODULE_LOG_LEVEL=4
	-DCONFIG_MEMFAULT=y
	-DCONFIG_WATCHDOG_APPLICATION=y
	-DCONFIG_DEBUG_MODULE=y
	-DCONFIG_DEBUG_MODULE_MEMFAULT_USE_EXTERNAL_TRANSPORT=y
	-DCONFIG_DEBUG_MODULE_MEMFAULT_WATCHDOG_DELTA_MS=5000
	-DCONFIG_DEBUG_MODULE_MEMFAULT_CHUNK_SIZE_MAX=80
	-DCONFIG_DEBUG_MODULE_MEMFAULT_THREAD_STACK_SIZE=256
)
