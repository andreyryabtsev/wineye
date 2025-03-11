#pragma once
#include <nvml.h>
#include <cstdint>
#include <string>

struct GPUInfo {
    uint64_t bytesFree;
    uint64_t bytesTotal;
    uint32_t utilization;
    int32_t temperature;
};

class GPUWatcher {
public:
    GPUWatcher();
    ~GPUWatcher();
    GPUInfo snapshot() const;
    std::string getDeviceName() const;
private:
    nvmlDevice_t device_;
};
