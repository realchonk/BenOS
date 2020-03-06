#ifndef FILE_ARCH_DRIVERS_HPP
#define FILE_ARCH_DRIVERS_HPP
#include <stddef.h>
#include <stdint.h>
#include "driver.hpp"
#include "mm.h"

namespace io {
	class KeyboardDriver : public mm::InterruptHandler, public io::Driver {
	public:
		KeyboardDriver() noexcept;

		bool setup() noexcept override;
		int reset() noexcept override;
		void handleInterrupt() noexcept override;
	};
	class MouseDriver : public mm::InterruptHandler, public io::Driver {
	private:
		uint8_t cycle;
		uint8_t buttons;
		int8_t buffer[3];
		bool enabled;
	public:
		float posx, posy;
		float xscale, yscale;
		MouseDriver() noexcept;

		bool setup() noexcept override;
		int reset() noexcept override;
		void handleInterrupt() noexcept override;
		void set_enabled(bool en) { enabled = en; }
		bool is_enabled() const { return enabled; }
	};
}

#endif /* FILE_ARCH_DRIVERS_HPP */