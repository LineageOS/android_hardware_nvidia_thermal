/*
 * Copyright (C) 2017 The Android Open Source Project
 * Copyright (C) 2017-2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.thermal@1.0-service-nvidia"

// #define LOG_NDEBUG 0

#include <android/log.h>
#include <utils/Log.h>
#include <math.h>
#include "Thermal.h"
#include "thermalhal.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

Thermal::Thermal() {
  ssize_t count = thermal_init();
  ALOGI("Found %zu thermal devices", count);
}

static inline TemperatureType ConvertTempType(enum temperature_type in) {
  TemperatureType out = TemperatureType::UNKNOWN;

  switch (in) {
    case DEVICE_TEMPERATURE_CPU:
      out = TemperatureType::CPU;
      break;
    case DEVICE_TEMPERATURE_GPU:
      out = TemperatureType::GPU;
      break;
    case DEVICE_TEMPERATURE_BATTERY:
      out = TemperatureType::BATTERY;
      break;
    case DEVICE_TEMPERATURE_SKIN:
      out = TemperatureType::SKIN;
      break;
    case DEVICE_TEMPERATURE_UNKNOWN:
      // Is the initial value
      break;
    default:
      ALOGE("Unknown temperature type %d", in);
      ;
  }

  return out;
}

static inline CoolingType ConvertCoolingType(enum cooling_type in) {
  CoolingType out = CoolingType::FAN_RPM;

  switch (in) {
    case FAN_RPM:
      // This is the initial value
      break;
    default:
      ALOGE("Unknown cooling type %d", in);
      // Unfortunately, there is no unknown enum value
      ;
  }

  return out;
}

static inline float finalizeTemperature(float temperature) {
    return temperature == UNKNOWN_TEMPERATURE ? NAN : temperature;
}

// Methods from ::android::hardware::thermal::V1_0::IThermal follow.
Return<void> Thermal::getTemperatures(getTemperatures_cb _hidl_cb) {
  ThermalStatus status;
  status.code = ThermalStatusCode::SUCCESS;
  hidl_vec<Temperature> temperatures;
  std::vector<temperature_t> temps;

  temps.resize(get_temperatures(NULL, 0));
  int res = get_temperatures(temps.data(), temps.size());
  if (res > 0) {
    temperatures.resize(res);
    for (size_t i = 0; i < res; ++i) {
      temperatures[i].type = ConvertTempType(temps[i].type);
      temperatures[i].name = temps[i].name;
      temperatures[i].currentValue = finalizeTemperature(temps[i].current_value);
      temperatures[i].throttlingThreshold = finalizeTemperature(temps[i].throttling_threshold);
      temperatures[i].shutdownThreshold = finalizeTemperature(temps[i].shutdown_threshold);
      temperatures[i].vrThrottlingThreshold =
              finalizeTemperature(temps[i].vr_throttling_threshold);
    }
  } else {
    status.code = ThermalStatusCode::FAILURE;
    status.debugMessage = strerror(-res);
  }

  _hidl_cb(status, temperatures);
  return Void();
}

Return<void> Thermal::getCpuUsages(getCpuUsages_cb _hidl_cb) {
  ThermalStatus status;
  status.code = ThermalStatusCode::SUCCESS;
  hidl_vec<CpuUsage> cpuUsages;
  std::vector<cpu_usage_t> usages;

  usages.resize(get_cpu_usages(NULL));
  int res = get_cpu_usages(usages.data());
  if (res > 0) {
    cpuUsages.resize(res);
    for (size_t i = 0; i < res; ++i) {
      cpuUsages[i].name = usages[i].name;
      cpuUsages[i].active = usages[i].active;
      cpuUsages[i].total = usages[i].total;
      cpuUsages[i].isOnline = usages[i].is_online;
    }
  } else {
    status.code = ThermalStatusCode::FAILURE;
    status.debugMessage = strerror(-res);
  }

  _hidl_cb(status, cpuUsages);
  return Void();
}

Return<void> Thermal::getCoolingDevices(getCoolingDevices_cb _hidl_cb) {
  ThermalStatus status;
  status.code = ThermalStatusCode::SUCCESS;
  hidl_vec<CoolingDevice> coolingDevices;
  std::vector<cooling_device_t> cdevs;

  cdevs.resize(get_cooling_devices(NULL, 0));
  int res = get_cooling_devices(cdevs.data(), cdevs.size());
  if (res > 0) {
    coolingDevices.resize(res);
    for (size_t i = 0; i < res; ++i) {
      coolingDevices[i].type = ConvertCoolingType(cdevs[i].type);
      coolingDevices[i].name = cdevs[i].name;
      coolingDevices[i].currentValue = cdevs[i].current_value;
    }
  } else {
    status.code = ThermalStatusCode::FAILURE;
    status.debugMessage = strerror(-res);
  }

  _hidl_cb(status, coolingDevices);
  return Void();
}

status_t Thermal::registerAsSystemService() {
    status_t ret = 0;

    ret = IThermal::registerAsService();
    if (ret != 0) {
        ALOGE("Failed to register IThermal (%d)", ret);
        goto fail;
    } else {
        ALOGI("Successfully registered IThermal");
    }

fail:
    return ret;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
