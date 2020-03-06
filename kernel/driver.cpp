#include "driver.hpp"
#include "util.h"

namespace io {
	static Driver* drivers[256]{};
	bool DriverManager::add(Driver* driver) {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			if (drivers[i] == driver) return true;
		}
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			if (!drivers[i]) return drivers[i] = driver, true;
		}
		return false;
	}
	bool DriverManager::remove(Driver* driver) {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			if (driver == drivers[i]) return drivers[i] = NULL, true;
		}
		return false;
	}

	void DriverManager::setupAll() {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			Driver* driver = drivers[i];
			if (driver) driver->setup();	
		}
	}
	void DriverManager::resetAll() {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			Driver* driver = drivers[i];
			if (driver) {
				const int n = driver->reset();

			}
		}
	}
	void DriverManager::destroyAll() {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			Driver* driver = drivers[i];
			if (driver) driver->destroy();	
		}
	}

	Driver* DriverManager::get(size_t ID) { return nullptr; }
	size_t DriverManager::num_drivers() {return 0;}
}