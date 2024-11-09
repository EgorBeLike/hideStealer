#pragma once

#include "Global.h"

//   PC INFO
struct MEMPLATE {
    const char* dimm;
    uint64_t memory;
    const char* ddr;
    uint64_t speed;
    const char* product;
};

struct MEMINFO {
    size_t used;
    size_t total;
    int percent;
    MEMPLATE* plates;
    size_t count_plates;
};

struct MBINFO {
    const char* vendor;
    const char* product;
};
struct CPUINFO {
    bool is64bit;
    const char* brand;
    size_t threads;
    size_t cores;
};
struct PCINFO {
    MEMINFO mem;
    MBINFO mb;
    CPUINFO cpu;
};
// ! PC INFO

// Return PC info in struct
PCINFO get_pcinfo()
{
    PCINFO info{};

    constexpr size_t oneGiB = 1024U * 1024U * 1024U;

    //   Get RAM info
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);

    const auto freeMem = static_cast<double>(ms.ullAvailPhys);
    const auto totalMem = static_cast<double>(ms.ullTotalPhys);

    const double percent = freeMem > 0 ? ((totalMem - freeMem) / totalMem * 100.0) : 100.0;

    MEMINFO mem{};
    mem.used = (totalMem - freeMem) / oneGiB;
    mem.total = totalMem / oneGiB;
    mem.percent = percent;

    // ! Get RAM info

    //   Get RAM plates info
    xmrig::DmiReader reader;

    if (!reader.read()) return info;

    const bool printEmpty = reader.memory().size() <= 8;

    vector<MEMPLATE> plates;

    for (const auto& memory : reader.memory()) {
        if (!memory.isValid()) {
            continue;
        }

        MEMPLATE plate{};

        plate.dimm = memory.id().data();
        plate.memory = memory.size() / oneGiB;
        plate.ddr = memory.type();
        plate.speed = memory.speed() / 1000000ULL;
        plate.product = memory.product().data();

        plates.push_back(plate);

    }

    mem.plates = plates.data();
    mem.count_plates = plates.size();

    // ! Get RAM plates info

    //   Get motherboard info

    const auto& board = xmrig::Cpu::info()->isVM() ? reader.system() : reader.board();
    MBINFO mb{};
    if (board.isValid()) {
        mb.vendor = board.vendor().data();
        mb.product = board.product().data();
    }

    CPUINFO cpu{};
    const auto cpu_ = xmrig::Cpu::info();
    cpu.is64bit = xmrig::ICpuInfo::is64bit();
    cpu.brand = cpu_->brand();
    cpu.threads = cpu_->threads();
    cpu.cores = cpu_->cores();

    info.mem = mem;
    info.mb = mb;
    info.cpu = cpu;

    // ! Get motherboard info

    return info;
}