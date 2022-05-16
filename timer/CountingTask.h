#pragma once

#include "timer/ITask.h"
#include <mutex>
#include <string>
#include <map>
#include <atomic>

class CountingTask : public ITask
{
public:
    CountingTask(const char* taskName);
    virtual ~CountingTask();

    void excute();

    void Add(std::string key, uint64_t value);
    void AddCurrent(uint64_t value);
    //void Clear(std::string key, int value);
    //void ClearAll();
private:
    std::mutex m_mutex;
    std::map<std::string, std::atomic<uint64_t>> m_Counters;
    std::atomic<uint64_t> m_Current;
    std::atomic<uint64_t> m_Total;
};
