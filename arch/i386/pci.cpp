#include "arch/i386/port.h"
#include "arch/i386/pci.h"
#include "driver.hpp"
#include "printk.h"
#include "util.h"
#include "mm.h"

#define PORT_DATA	0x0cfc
#define PORT_CMD	0x0cf8

static pci_device_driver_info_t device_drivers[256]{};
static pci_class_driver_info_t class_drivers[256]{};
bool pci_register_device_driver(pci_device_driver_info_t info) {
	size_t i;
	for (i = 0; i < arraylen(device_drivers); ++i) {
		if (device_drivers[i].vendorID == info.vendorID
			&& device_drivers[i].deviceID == info.deviceID) return false;
	}
	if (i >= arraylen(device_drivers)) return false;
	device_drivers[i] = info;
	return true;
}
bool pci_register_class_driver(pci_class_driver_info_t info) {
	size_t i;
	for (i = 0; i < arraylen(class_drivers); ++i) {
		if (class_drivers[i].classID == info.classID
			&& class_drivers[i].subclassID == info.subclassID) return false;
	}
	if (i >= arraylen(class_drivers)) return false;
	class_drivers[i & 0xff] = info;
	return true;
}

static io::Driver* get_driver(const pci_device_info_t& info) {
	for (size_t i = 0; i < arraylen(device_drivers) && device_drivers[i].driver; ++i) {
		if (device_drivers[i].vendorID == info.vendorID
			&& device_drivers[i].deviceID == info.deviceID)
			return device_drivers[i].driver;
	}
	for (size_t i = 0; i < arraylen(class_drivers) && class_drivers[i].driver; ++i) {
		if (class_drivers[i].classID == info.classID
			&& class_drivers[i].subclassID == info.subclassID)
			return device_drivers[i].driver;
	}
	return nullptr;
}
extern "C" {
int pci_init(void) {
	log(DEBUG, "pci_init();");
	for (uint8_t bus = 0; bus < 8; ++bus) {
		for (uint8_t dev = 0; dev < 32; ++dev) {
			const uint8_t nfunc = pci_is_connected(bus, dev) ? 8 : 1;
			for (uint8_t f = 0; f < nfunc; ++f) {
				const pci_address_t addr = { bus, dev, f };
				pci_device_info_t info = pci_get_device(addr);
				if (!info.vendorID || info.vendorID == 0xffff) continue;
				for (uint8_t i = 0; i < 6; ++i) {
					const pci_base_addr_reg_t bar = pci_get_bar(addr, i);
					if (bar.ptr && bar.type) {
						info.port_base = (uint32_t)bar.ptr;
						io::Driver* const drv = get_driver(info);
						if (drv) io::DriverManager::add(drv);
					}
				}
				log(DEBUG, "PCI BUS %$a%d%$r, DEVICE %$b%d%$r, FUNCTION %$e%d%$r, VENDOR %$c%04x%$r, DEVICE %$d%04x%$r",
					bus, dev, f, info.vendorID, info.deviceID);
			}
		}
	}
	return 0;
}
void pci_write(pci_address_t addr, uint8_t regOffset, uint32_t value) {
	const uint32_t a = (1 << 31)
		| (addr.bus << 16)
		| (addr.device << 11)
		| (addr.function << 8)
		| (regOffset & 0xfc);
	outl(PORT_CMD, a);
	outl(PORT_DATA, value);
}
uint32_t pci_read(pci_address_t addr, uint8_t regOffset) {
	const uint32_t a = (1 << 31)
		| (addr.bus << 16)
		| (addr.device << 11)
		| (addr.function << 8)
		| (regOffset & 0xfc);
	outl(PORT_CMD, a);
	return inl(PORT_DATA) >> (8 * (regOffset % 4));
}
bool pci_is_connected(uint8_t bus, uint8_t device) {
	return pci_read((pci_address_t){ bus, device, 0 }, 0x0e) & 0x80;
}
pci_device_info_t pci_get_device(pci_address_t addr) {
	pci_device_info_t info;
	info.addr = addr;
	info.vendorID =		pci_read(addr, 0x00);
	info.deviceID = 	pci_read(addr, 0x02);
	info.classID = 		pci_read(addr, 0x0b);
	info.subclassID = 	pci_read(addr, 0x0a);
	info.interfaceID = 	pci_read(addr, 0x09);
	info.revision = 	pci_read(addr, 0x08);
	info.interrupt =	pci_read(addr, 0x3c);
	return info;
}
pci_base_addr_reg_t pci_get_bar(pci_address_t addr, uint8_t bar) {
	pci_base_addr_reg_t r;
	if (bar >= (6 - (4 * pci_read(addr, 0x0e) & 0x7f))) return r;
	const uint32_t barval = pci_read(addr, 0x10 + 4 * bar);
	r.type = barval & 1;
	if (r.type) {
		r.prefetchable = (barval >> 3) & 1;
		switch ((barval >> 1) & 3) {
		case 0b00: // 32bit
		case 0b01: // 20bit
		case 0b10: // 64bit
			break;
		}
	}
	else {
		r.ptr = (volatile uint8_t*)(barval & ~3);
		r.prefetchable = false;
	}
	return r;
}
}