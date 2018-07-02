#pragma once
#include <atomic>
#include <thread>

class NonblockingInterface
{
protected:
	std::atomic_flag flag;
	void lock_cas();

	void unlock();

	bool lock();
};
