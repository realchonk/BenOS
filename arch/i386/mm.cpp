#include <stdint.h>
#include "arch/i386/x86task.h"
#include "arch/i386/port.h"
#include "printk.h"
#include "mm.h"

namespace mm {
	struct SegDescriptor {
		uint16_t limit_lo;
		uint16_t base_lo;
		uint8_t base_hi;
		uint8_t type;
		uint8_t flags_limit_hi;
		uint8_t base_vhi;
	} __attribute__((packed));
	static struct {
		SegDescriptor nullSegment;
		SegDescriptor codeSegment;
		SegDescriptor dataSegment;
		SegDescriptor ucodeSegment;
		SegDescriptor udataSegment;
		SegDescriptor tssSegment;
	} gdt;

	static SegDescriptor make_segdescriptor(uint32_t base, uint32_t limit, uint8_t type) {
		SegDescriptor seg;
		if (limit <= 65536)
			seg.flags_limit_hi = 0x40;
		else {
			if ((limit & 0xfff) != 0xfff) limit = (limit >> 12) - 1;
			else limit >>= 12;
			seg.flags_limit_hi = 0xc0;
		}
		seg.limit_lo = limit & 0xffff;
		seg.flags_limit_hi |= (limit & 0xf0000) >> 16;
		seg.base_lo = base & 0xffff;
		seg.base_hi = (base >> 16) & 0xff;
		seg.base_vhi = (base >> 24) & 0xff;
		seg.type = type;
		return seg;
	}
	/*static uint32_t segdescriptor_base(const SegDescriptor seg) {
		uint32_t base = 0;
		base |= seg.base_lo;
		base |= seg.base_hi << 16;
		base |= seg.base_vhi << 24;
		return base;
	}
	static uint32_t segdescriptor_limit(const SegDescriptor seg) {
		uint32_t limit = 0;
		limit |= seg.limit_lo;
		limit |= (seg.flags_limit_hi & 0xf) << 16;
		if ((seg.flags_limit_hi & 0xc0) == 0xc0)
			limit = (limit << 12) | 0xfff;
		return limit;
	}*/
	extern "C" {
		uint16_t i386_gdt_codeSegmentSelector(void) {
			return (const volatile uint8_t*)&gdt.codeSegment - (const volatile uint8_t*)&gdt;
		}
		uint16_t i386_gdt_dataSegmentSelector(void) {
			return (const volatile uint8_t*)&gdt.dataSegment - (const volatile uint8_t*)&gdt;
		}
		uint16_t i386_gdt_ucodeSegmentSelector(void) {
			return (const volatile uint8_t*)&gdt.ucodeSegment - (const volatile uint8_t*)&gdt;
		}
		uint16_t i386_gdt_udataSegmentSelector(void) {
			return (const volatile uint8_t*)&gdt.udataSegment - (const volatile uint8_t*)&gdt;
		}
	}
	static int gdt_init(void) {
		gdt.nullSegment = make_segdescriptor(0, 0, 0);
		gdt.codeSegment = make_segdescriptor(0, 0xffffff, 0x9a);
		gdt.dataSegment =  make_segdescriptor(0, 0xffffff, 0x92);
		gdt.ucodeSegment = make_segdescriptor(0, 0xffffff, 0xfa);
		gdt.udataSegment =  make_segdescriptor(0, 0xffffff, 0xf2);
		gdt.tssSegment = make_segdescriptor(0, 0, 0);
	 
		const uint32_t i[2] = { sizeof(gdt) << 16, (uint32_t)&gdt };
		asm volatile("lgdt (%0)" : : "p"((uint8_t*)i + 2));

		asm volatile("mov $0x10, %ax");
		asm volatile("mov %ax, %ds");
		asm volatile("mov %ax, %es");
		asm volatile("mov %ax, %fs");
		asm volatile("mov %ax, %gs");
		asm volatile("mov %ax, %ss");
		asm volatile("ljmp $0x8, $.1\n.1:");

		return 0;
	}

	struct Gate {
		uint16_t handler_lo;
		uint16_t cs;
		uint8_t reserved;
		uint8_t access;
		uint16_t handler_hi;
	} __attribute__((packed));
	typedef struct Gate Gate;

	static volatile Gate idt[256];
	static void set_idt_entry(uint8_t num, uint16_t cs, void(*handler)(), uint8_t previlege, uint8_t type) {
		idt[num].handler_hi = ((uint32_t)handler) >> 16;
		idt[num].handler_lo = ((uint32_t)handler) & 0xffff;
		idt[num].cs = cs;
		idt[num].access = 0x80 | (type & 0xf) | ((previlege & 0x3) << 5);
		idt[num].reserved = 0;
	}

	extern "C" {
	extern void ignore_ireq(void);
	extern void handle_ireq0x00(void);
	extern void handle_ireq0x01(void);
	extern void handle_ireq0x02(void);
	extern void handle_ireq0x03(void);
	extern void handle_ireq0x04(void);
	extern void handle_ireq0x05(void);
	extern void handle_ireq0x06(void);
	extern void handle_ireq0x07(void);
	extern void handle_ireq0x08(void);
	extern void handle_ireq0x09(void);
	extern void handle_ireq0x0A(void);
	extern void handle_ireq0x0B(void);
	extern void handle_ireq0x0C(void);
	extern void handle_ireq0x0D(void);
	extern void handle_ireq0x0E(void);
	extern void handle_ireq0x0F(void);
	extern void handle_ireq0xA0(void);
	extern void handle_excep0x00(void);
	extern void handle_excep0x01(void);
	extern void handle_excep0x02(void);
	extern void handle_excep0x03(void);
	extern void handle_excep0x04(void);
	extern void handle_excep0x05(void);
	extern void handle_excep0x06(void);
	extern void handle_excep0x07(void);
	extern void handle_excep0x08(void);
	extern void handle_excep0x09(void);
	extern void handle_excep0x0A(void);
	extern void handle_excep0x0B(void);
	extern void handle_excep0x0C(void);
	extern void handle_excep0x0D(void);
	extern void handle_excep0x0E(void);
	extern void handle_excep0x0F(void);
	extern void handle_excep0x10(void);
	extern void handle_excep0x11(void);
	extern void handle_excep0x12(void);
	extern void handle_excep0x13(void);
	extern void handle_excep0x14(void);
	extern void handle_excep0x15(void);
	extern void handle_excep0x16(void);
	extern void handle_excep0x17(void);
	extern void handle_excep0x18(void);
	extern void handle_excep0x19(void);
	extern void handle_excep0x1A(void);
	extern void handle_excep0x1B(void);
	extern void handle_excep0x1C(void);
	extern void handle_excep0x1D(void);
	extern void handle_excep0x1E(void);
	extern void handle_excep0x1F(void);
	}
	static int idt_init(void) {
		const volatile uint32_t cs = i386_gdt_codeSegmentSelector();
		for (uint16_t i = 0x30; i < 0x100; ++i) {
			set_idt_entry(i, cs, ignore_ireq, 0, 0xe);
		}

		set_idt_entry(0x00, cs, handle_excep0x00, 0, 0xe);
		set_idt_entry(0x01, cs, handle_excep0x01, 0, 0xe);
		set_idt_entry(0x02, cs, handle_excep0x02, 0, 0xe);
		set_idt_entry(0x03, cs, handle_excep0x03, 0, 0xe);
		set_idt_entry(0x04, cs, handle_excep0x04, 0, 0xe);
		set_idt_entry(0x05, cs, handle_excep0x05, 0, 0xe);
		set_idt_entry(0x06, cs, handle_excep0x06, 0, 0xe);
		set_idt_entry(0x07, cs, handle_excep0x07, 0, 0xe);
		set_idt_entry(0x08, cs, handle_excep0x08, 0, 0xe);
		set_idt_entry(0x09, cs, handle_excep0x09, 0, 0xe);
		set_idt_entry(0x0a, cs, handle_excep0x0A, 0, 0xe);
		set_idt_entry(0x0b, cs, handle_excep0x0B, 0, 0xe);
		set_idt_entry(0x0c, cs, handle_excep0x0C, 0, 0xe);
		set_idt_entry(0x0d, cs, handle_excep0x0D, 0, 0xe);
		set_idt_entry(0x0e, cs, handle_excep0x0E, 0, 0xe);
		set_idt_entry(0x0f, cs, handle_excep0x0F, 0, 0xe);
		set_idt_entry(0x10, cs, handle_excep0x10, 0, 0xe);
		set_idt_entry(0x11, cs, handle_excep0x11, 0, 0xe);
		set_idt_entry(0x12, cs, handle_excep0x12, 0, 0xe);
		set_idt_entry(0x13, cs, handle_excep0x13, 0, 0xe);
		set_idt_entry(0x14, cs, handle_excep0x14, 0, 0xe);
		set_idt_entry(0x15, cs, handle_excep0x15, 0, 0xe);
		set_idt_entry(0x16, cs, handle_excep0x16, 0, 0xe);
		set_idt_entry(0x17, cs, handle_excep0x17, 0, 0xe);
		set_idt_entry(0x18, cs, handle_excep0x18, 0, 0xe);
		set_idt_entry(0x19, cs, handle_excep0x19, 0, 0xe);
		set_idt_entry(0x1a, cs, handle_excep0x1A, 0, 0xe);
		set_idt_entry(0x1b, cs, handle_excep0x1B, 0, 0xe);
		set_idt_entry(0x1c, cs, handle_excep0x1C, 0, 0xe);
		set_idt_entry(0x1d, cs, handle_excep0x1D, 0, 0xe);
		set_idt_entry(0x1e, cs, handle_excep0x1E, 0, 0xe);
		set_idt_entry(0x1f, cs, handle_excep0x1F, 0, 0xe);
		set_idt_entry(0x20, cs, handle_ireq0x00, 0, 0xe);
		set_idt_entry(0x21, cs, handle_ireq0x01, 0, 0xe);
		set_idt_entry(0x22, cs, handle_ireq0x02, 0, 0xe);
		set_idt_entry(0x23, cs, handle_ireq0x03, 0, 0xe);
		set_idt_entry(0x24, cs, handle_ireq0x04, 0, 0xe);
		set_idt_entry(0x25, cs, handle_ireq0x05, 0, 0xe);
		set_idt_entry(0x26, cs, handle_ireq0x06, 0, 0xe);
		set_idt_entry(0x27, cs, handle_ireq0x07, 0, 0xe);
		set_idt_entry(0x28, cs, handle_ireq0x08, 0, 0xe);
		set_idt_entry(0x29, cs, handle_ireq0x09, 0, 0xe);
		set_idt_entry(0x2a, cs, handle_ireq0x0A, 0, 0xe);
		set_idt_entry(0x2b, cs, handle_ireq0x0B, 0, 0xe);
		set_idt_entry(0x2c, cs, handle_ireq0x0C, 0, 0xe);
		set_idt_entry(0x2d, cs, handle_ireq0x0D, 0, 0xe);
		set_idt_entry(0x2e, cs, handle_ireq0x0E, 0, 0xe);
		set_idt_entry(0x2f, cs, handle_ireq0x0F, 0, 0xe);
		set_idt_entry(0xa0, cs, handle_ireq0xA0, 0, 0xe);

		outb_slow(0x20, 0x11);
		outb_slow(0xa0, 0x11);

		outb_slow(0x21, 0x20);
		outb_slow(0xa1, 0x28);

		outb_slow(0x21, 0x04);
		outb_slow(0xa1, 0x02);

		outb_slow(0x21, 0x01);
		outb_slow(0xa1, 0x01);

		outb_slow(0x21, 0x00);
		outb_slow(0xa1, 0x00);

		const volatile uint32_t i[2] = { (256 * sizeof(Gate) - 1) << 16, (uint32_t)&idt };
		asm volatile("lidt (%0)" : : "p"((uint8_t*)i + 2));

		return 0;
	}
	// PIC master command 0x20
	// PIC master data    0x21
	// PIC slave  command 0xA0
	// PIC slave  data    0xA1


	static InterruptHandler* int_handlers[256];

	extern "C" struct cpu_state* taskmgr_schedule(struct cpu_state* cpustate);
	extern "C"
	struct cpu_state* handle_interrupt(uint32_t num, uint32_t err, struct cpu_state* cpu) {
		//printk("i");
		if (num >= 0x20 && num < 0x30) {
			outb_slow(0x20, 0x20);
			if (num >= 0x28) outb_slow(0xa0, 0x20);
		}
		switch (num) {
		case 0x00: printk("%p: division by zero", cpu->eip); break;
		case 0x06: panic("%p: invalid opcode", cpu->eip); break;
		case 0x05: panic("%p: out of bounds", cpu->eip); break;
		case 0x0d: panic("%p: general protection fault, err=%d", cpu->eip, err); break;
		case 0x0e: panic("%p: page fault"); break;
		case 0x20: cpu = taskmgr_schedule(cpu); break;
		default:
			if (int_handlers[num])
				int_handlers[num]->handleInterrupt();
		}

		return cpu;
	}


	InterruptHandler::~InterruptHandler() noexcept {
		if (int_handlers[interrupt] == this) int_handlers[interrupt] = nullptr;
	}
	void InterruptHandler::registerSelf() noexcept {
		int_handlers[interrupt] = this;
	}
}


extern "C" {
void mm_mmu_init(void);
int mm_init(void) {
	log(DEBUG, "mm_init();");
	mm::gdt_init();
	mm::idt_init();

	return 0;
}
}