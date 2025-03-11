#include <iostream>
#include <nvml.h>
#include "gpu.h"

void checkNvmlError(nvmlReturn_t error, const char* message) {
    if (error != NVML_SUCCESS) {
        std::cerr << message << ": " << nvmlErrorString(error) << std::endl;
        exit(EXIT_FAILURE);
    }
}

GPUWatcher::GPUWatcher() {
    nvmlReturn_t result = nvmlInit();
    checkNvmlError(result, "Failed to initialize NVML");
    result = nvmlDeviceGetHandleByIndex(0, &device_);
    checkNvmlError(result, "Failed to get device handle");
}

GPUWatcher::~GPUWatcher() {
    checkNvmlError(nvmlShutdown(), "Failed to shutdown NVML");
}

std::string GPUWatcher::getDeviceName() const {
    char name[NVML_DEVICE_NAME_V2_BUFFER_SIZE];
    checkNvmlError(nvmlDeviceGetName(device_, name, NVML_DEVICE_NAME_V2_BUFFER_SIZE), "Failed to get device name");
    return std::string(name);
}

GPUInfo GPUWatcher::snapshot() const {
    // Get GPU utilization
    nvmlUtilization_t utilization;
    checkNvmlError(nvmlDeviceGetUtilizationRates(device_, &utilization), "Failed to get utilization rates");
    // Get memory info
    nvmlMemory_t memory;
    checkNvmlError(nvmlDeviceGetMemoryInfo(device_, &memory), "Failed to get memory info");
    // Get temperature
    nvmlGpuThermalSettings_t tempInfo;
    checkNvmlError(nvmlDeviceGetThermalSettings(device_, 0, &tempInfo), "Failed to get temperatures");
    const int currentTemp = tempInfo.sensor[0].currentTemp;

    return GPUInfo{
      /*bytesFree=*/ memory.free,
      /*bytesTotal=*/ memory.total,
      /*utilization=*/ utilization.gpu,
      /*temperature=*/ currentTemp
    };
}
