#pragma once
#include <string>

class ITask
{
public:
	ITask(const char* taskName) :m_Name(taskName)
	{

	}
	virtual const char* GetName() final
	{
		return m_Name.c_str();
	};
	virtual void excute() = 0;
private:
	std::string m_Name;
};