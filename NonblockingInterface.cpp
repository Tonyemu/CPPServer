#include "NonblockingInterface.h"

void NonblockingInterface::lock_cas()
{
	while (flag.test_and_set())std::this_thread::yield();
}

void NonblockingInterface::unlock()
{
	flag.clear();
}

bool NonblockingInterface::lock()
{
	return !flag.test_and_set();
}
