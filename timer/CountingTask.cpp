#include <common_header.h>

#include "CountingTask.h"

CountingTask::CountingTask(const char* taskName): ITask(taskName)
{
}

CountingTask::~CountingTask()
{
}

void CountingTask::excute()
{
	//std::lock_guard<std::mutex> lock(m_mutex);
	uint64_t current_local = m_Current.load(std::memory_order_acquire);
	m_Total += current_local;
	m_Current.store(0, std::memory_order_release);
	LogFile::Info("[%s] current consumed cnt : %llu total consumed cnt : %llu", ITask::GetName(), current_local, m_Total.load());
}

void CountingTask::Add(std::string key, uint64_t value)
{
	m_Counters[key] += value;
	m_Counters["total"] += value;
}

void CountingTask::AddCurrent(uint64_t value)
{
	//std::lock_guard<std::mutex> lock(m_mutex);
	m_Current.fetch_add(value, memory_order_acq_rel);
}