/*
 * Copyright (C) 2017 The Android Open Source Project
 * Copyright (C) 2024 The LineageOS Project
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

#define LOG_TAG "android.hardware.thermal-service-nvidia"

#include <android/log.h>
#include <utils/Log.h>
#include <math.h>
#include "Thermal-aidl.h"
#include "thermalhal.h"

namespace aidl {
namespace android {
namespace hardware {
namespace thermal {
namespace impl {
namespace nvidia {

Thermal::Thermal() {
  ssize_t count = thermal_init();
  ALOGI("Found %zu thermal devices", count);
}

static inline CoolingType ConvertCoolingType(enum cooling_type in) {
  CoolingType out = CoolingType::FAN;

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

ndk::ScopedAStatus Thermal::getCoolingDevices(std::vector<CoolingDevice>* coolingDevices) {
  std::vector<cooling_device_t> cdevs;

  cdevs.resize(get_cooling_devices(NULL, 0));
  int res = get_cooling_devices(cdevs.data(), cdevs.size());
  if (res > 0) {
    coolingDevices->resize(res);
    for (size_t i = 0; i < res; ++i) {
      coolingDevices->at(i).type = ConvertCoolingType(cdevs[i].type);
      coolingDevices->at(i).name = cdevs[i].name;
      coolingDevices->at(i).value = cdevs[i].current_value;
    }
  } else {
    return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_SERVICE_SPECIFIC, strerror(-res));
  }

  return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getCoolingDevicesWithType(CoolingType in_type,
                                                 std::vector<CoolingDevice>* coolingDevices) {
  ndk::ScopedAStatus ret = getCoolingDevices(coolingDevices);
  if (!ret.isOk())
    return ret;

  for (auto it = coolingDevices->begin(); it != coolingDevices->end();) {
    if (it->type != in_type)
      it = coolingDevices->erase(it);
    else
      it++;
  }

  return ndk::ScopedAStatus::ok();
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

static inline float finalizeTemperature(float temperature) {
    return temperature == UNKNOWN_TEMPERATURE ? NAN : temperature;
}

ndk::ScopedAStatus Thermal::getTemperatures(std::vector<Temperature>* temperatures) {
  std::vector<temperature_t> temps;

  temps.resize(get_temperatures(NULL, 0));
  int res = get_temperatures(temps.data(), temps.size());
  if (res > 0) {
    temperatures->resize(res);
    for (size_t i = 0; i < res; ++i) {
      temperatures->at(i).type = ConvertTempType(temps[i].type);
      temperatures->at(i).name = temps[i].name;
      temperatures->at(i).value = finalizeTemperature(temps[i].current_value);
      temperatures->at(i).throttlingStatus = ThrottlingSeverity::NONE; // TODO: Set this properly
    }
  } else {
    return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_SERVICE_SPECIFIC, strerror(-res));
  }

  return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getTemperaturesWithType(TemperatureType in_type,
                                               std::vector<Temperature>* temperatures) {
  ndk::ScopedAStatus ret = getTemperatures(temperatures);
  if (!ret.isOk())
    return ret;

  for (auto it = temperatures->begin(); it != temperatures->end();) {
    if (it->type != in_type)
      it = temperatures->erase(it);
    else
      it++;
  }

  return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getTemperatureThresholds(
        std::vector<TemperatureThreshold>* /* out_temperatureThresholds */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getTemperatureThresholdsWithType(
        TemperatureType /* in_type */,
        std::vector<TemperatureThreshold>* /* out_temperatureThresholds */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::registerThermalChangedCallback(
        const std::shared_ptr<IThermalChangedCallback>& /* in_callback */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::registerThermalChangedCallbackWithType(
        const std::shared_ptr<IThermalChangedCallback>& /* in_callback */, TemperatureType /* in_type */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::unregisterThermalChangedCallback(
        const std::shared_ptr<IThermalChangedCallback>& /* in_callback */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::registerCoolingDeviceChangedCallbackWithType(
        const std::shared_ptr<ICoolingDeviceChangedCallback>& /* in_callback */, CoolingType /* in_type */) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::unregisterCoolingDeviceChangedCallback(
        const std::shared_ptr<ICoolingDeviceChangedCallback>& /* in_callback */) {
    return ndk::ScopedAStatus::ok();
}

}  // namespace nvidia
}  // namespace impl
}  // namespace thermal
}  // namespace hardware
}  // namespace android
}  // namespace aidl
