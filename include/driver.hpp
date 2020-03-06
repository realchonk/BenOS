#ifndef FILE_DRIVER_HPP
#define FILE_DRIVER_HPP
#include <stddef.h>
#include <stdint.h>

namespace io {
	class Driver {
	private:
		friend class DriverManager;
		size_t driver_id;
	public:
		Driver() noexcept {}
		~Driver() noexcept { destroy(); }

		virtual bool setup() noexcept { return false; }
		virtual int reset() noexcept { return 0; }
		virtual void destroy() noexcept {}
		size_t driverID() const noexcept { return driver_id; }
	};

	class DriverManager {
	public:
		static bool add(Driver* driver);
		static bool remove(Driver* driver);

		static void setupAll();
		static void resetAll();
		static void destroyAll();

		static Driver* get(size_t ID);
		static size_t num_drivers();
	};
}

#endif /* FILE_DRIVER_HPP */