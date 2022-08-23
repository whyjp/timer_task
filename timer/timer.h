#pragma once
#include "ITask.h"

#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Timer
{
public:
	Timer();
	virtual ~Timer();
	void Start();
	void Stop();
	void AddTaskSchedule(ITask* task, unsigned int periodSec);
	std::function<void(ITask*, unsigned int)> GetAddTasker();
private:
	void MainWorker();
	void TaskWorker(ITask* task, std::shared_ptr<std::condition_variable > cv);
private:
	std::atomic_bool m_Working;
	std::chrono::high_resolution_clock::time_point m_Startedtime;
	std::thread m_MainWorker;
	std::vector<std::thread> m_TaskWorkers;
	std::mutex m_ContainerMutex;

	std::map<unsigned int, std::vector<ITask*>> m_Tasks;

	std::map<ITask*, std::shared_ptr<std::mutex>> m_TasksMutex;
	std::map<unsigned int, std::shared_ptr<std::condition_variable>> m_PeriodCV;
	;
};