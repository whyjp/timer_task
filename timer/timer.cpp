#include "Timer.h"
#include <functional>
#include <time.h>
#include <condition_variable>

Timer::Timer() :m_Working(false)
{
}

Timer::~Timer()
{
	Stop();
}

void Timer::Start()
{
	m_Startedtime = std::chrono::high_resolution_clock::now();
	m_Working.store(true);
	m_MainWorker = std::thread(std::bind(&Timer::MainWorker, this));
}

void Timer::Stop()
{
	m_Working.store(false);

	if (m_MainWorker.joinable()) {
		m_MainWorker.join();
	}
	{
		std::lock_guard<std::mutex> lock(m_ContainerMutex);
		for (auto& cur : m_PeriodCV) {
			cur.second->notify_all();
		}
		for (auto& cur : m_TaskWorkers) {
			if (cur.joinable()) {
				cur.join();
			}
		}
	}
}

void Timer::AddTaskSchedule(ITask* task, unsigned int periodSec)
{
	if (task != nullptr && periodSec > 0) {
		{
			std::lock_guard<std::mutex> lock(m_ContainerMutex);
			m_Tasks[periodSec].push_back(task);

			m_TasksMutex.emplace(task, new std::mutex());
			auto cv = m_PeriodCV.find(periodSec);
			if (cv == m_PeriodCV.end()) {
				m_PeriodCV.emplace(periodSec, new std::condition_variable());
			}

			m_TaskWorkers.emplace_back(std::bind(&Timer::TaskWorker, this, task, m_PeriodCV[periodSec]));
		}
	}
}

std::function<void(ITask*, unsigned int)> Timer::GetAddTasker()
{
	return std::bind(&Timer::AddTaskSchedule, this, std::placeholders::_1, std::placeholders::_2);
}

void Timer::MainWorker()
{
	while (m_Working) {
		auto currenttime = std::chrono::high_resolution_clock::now();
		std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(currenttime - m_Startedtime);
		
		{
			std::lock_guard<std::mutex> lock(m_ContainerMutex);
			for (auto iter = m_Tasks.begin(); iter != m_Tasks.end(); ++iter) {
				if ((sec % iter->first) == (std::chrono::seconds)0) {					
					m_PeriodCV[iter->first]->notify_all();
				}
			}
		}

		std::this_thread::sleep_until(currenttime + std::chrono::seconds(1) - std::chrono::microseconds(6800));
	}
}

void Timer::TaskWorker(ITask* task, std::shared_ptr<std::condition_variable> cv)
{
	std::unique_lock<std::mutex> lock(*m_TasksMutex[task]);
	cv->wait(lock);
	while (m_Working) {
		task->excute();
		cv->wait(lock);
	}
}