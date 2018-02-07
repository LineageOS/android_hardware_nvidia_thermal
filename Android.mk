# Copyright (C) 2015 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifeq ($(TARGET_THERMALHAL_VARIANT),tegra)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := thermal.c

ifeq ($(TARGET_TEGRA_VERSION),t114)
    LOCAL_SRC_FILES += thermal.t114.c
else ifeq ($(TARGET_TEGRA_VERSION),t124)
    LOCAL_SRC_FILES += thermal.t124.c
else ifeq ($(TARGET_TEGRA_VERSION),t210)
    LOCAL_SRC_FILES += thermal.t210.c
endif

LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_MODULE := thermal.$(TARGET_BOARD_PLATFORM)

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

endif # TARGET_THERMALHAL_VARIANT == tegra
