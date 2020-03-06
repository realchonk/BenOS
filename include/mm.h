#ifndef FILE_MM_H
#define FILE_MM_H
#include <stdint.h>
#include "heap.hpp"

#ifdef __cplusplus
namespace mm {
	class InterruptHandler {
	public:
		const uint8_t interrupt;
		constexpr InterruptHandler(const uint8_t num) noexcept
			: interrupt(num) {}
		~InterruptHandler() noexcept;

		virtual void handleInterrupt() noexcept {}

		void registerSelf() noexcept;
	};
	extern HeapManager* heapmgr;
}

extern "C" {
#endif

int mm_init(void);
void enable_interrupts(void);
void disable_interrupts(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_MM_H */