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

#ifndef ANDROID_HARDWARE_THERMAL_V1_0_THERMAL_H
#define ANDROID_HARDWARE_THERMAL_V1_0_THERMAL_H

#include <android/hardware/thermal/1.0/IThermal.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <hardware/thermal.h>
#include "thermalhal.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

using ::android::hardware::thermal::V1_0::CoolingDevice;
using ::android::hardware::thermal::V1_0::CpuUsage;
using ::android::hardware::thermal::V1_0::IThermal;
using ::android::hardware::thermal::V1_0::Temperature;
using ::android::hardware::thermal::V1_0::ThermalStatus;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct Thermal : public IThermal {
    // Methods from ::android::hardware::thermal::V1_0::IThermal follow.

    Thermal();
    status_t registerAsSystemService();

    Return<void> getTemperatures(getTemperatures_cb _hidl_cb)  override;
    Return<void> getCpuUsages(getCpuUsages_cb _hidl_cb)  override;
    Return<void> getCoolingDevices(getCoolingDevices_cb _hidl_cb)  override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_THERMAL_V1_0_THERMAL_H
