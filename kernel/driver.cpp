#include "string.h"
#include "driver.hpp"
#include "util.h"

#include "printk.h"

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
				(void)n;
			}
		}
	}
	void DriverManager::destroyAll() {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			Driver* driver = drivers[i];
			if (driver) driver->destroy();	
		}
	}
	Driver* DriverManager::find(DriverType type) {
		for (size_t i = 0; i < arraylen(drivers); ++i) {
			Driver* const drv = drivers[i];
			if ((drv != nullptr) && (drv->type == type)) return drv;
		}
		return nullptr;
	}	

	Driver* DriverManager::get(size_t ID) { return drivers[ID]; }
	size_t DriverManager::num_drivers() { return 256; }
}