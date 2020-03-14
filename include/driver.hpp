#ifndef FILE_DRIVER_HPP
#define FILE_DRIVER_HPP
#include <stddef.h>
#include <stdint.h>
#include "mm.h"

namespace io {
	enum class DriverType {
		UNKNOWN		= 0,

		KEYBOARD	= 100,
		MOUSE		= 101,
	};
	class Driver {
	private:
		friend class DriverManager;
		size_t driver_id;
	public:
		const DriverType type;
		Driver(DriverType type) noexcept : type(type) {}
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
		static Driver* find(DriverType type);
	};

	// PLATFORM INDEPENDENT DRIVERS

	class KeyboardDriver : public mm::InterruptHandler, public Driver {
	public:
		using handler_t = void(*)(uint8_t scancode, int vkey, uint16_t mod, bool pressed);
		handler_t on_key{};
		KeyboardDriver() noexcept;

		bool setup() noexcept override;
		int reset() noexcept override;
		void handleInterrupt() noexcept override;
		static int transform(uint8_t scancode, uint16_t mod);
	};
	class MouseDriver : public mm::InterruptHandler, public Driver {
	private:
		uint8_t cycle;
		uint8_t buttons;
		int8_t buffer[3];
		bool enabled;
	public:
		using on_mousemove_t = void(*)(float x, float y, float dx, float dy);
		using on_mousebutton_t = void(*)(float x, float y, int btn, bool pressed);
		on_mousemove_t on_mousemove{};
		on_mousebutton_t on_mousebutton{};
		float posx, posy;
		float xscale, yscale;
		MouseDriver() noexcept;

		bool setup() noexcept override;
		int reset() noexcept override;
		void handleInterrupt() noexcept override;
		void set_enabled(bool en) { enabled = en; }
		bool is_enabled() const { return enabled; }
		bool get_button(int n) const { return buttons & (1 << n); }
	};
}

#endif /* FILE_DRIVER_HPP */