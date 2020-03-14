#ifndef FILE_ARCH_PCI_H
#define FILE_ARCH_PCI_H
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#include "driver.hpp"
extern "C" {
typedef struct pci_device_driver_info {
	io::Driver* driver;
	uint16_t vendorID;
	uint16_t deviceID;
} pci_device_driver_info_t;
typedef struct pci_class_driver_info {
	io::Driver* driver;
	uint8_t classID;
	uint8_t subclassID;
} pci_class_driver_info_t;
bool pci_register_device_driver(pci_device_driver_info_t info);
bool pci_register_class_driver(pci_class_driver_info_t info);
#endif
typedef struct pci_address {
	uint8_t bus : 3;
	uint8_t device : 5;
	uint8_t function : 3;
} pci_address_t;

typedef struct pci_device_info {
	pci_address_t addr;
	uint32_t port_base;
	uint32_t interrupt;
	uint16_t vendorID;
	uint16_t deviceID;
	uint8_t classID;
	uint8_t subclassID;
	uint8_t interfaceID;
	uint8_t revision;
} pci_device_info_t;

typedef struct pci_base_addr_reg {
	volatile uint8_t* ptr;
	uint32_t size;
	bool prefetchable;
	bool type;
} pci_base_addr_reg_t;

int pci_init(void);
void pci_write(pci_address_t addr, uint8_t regOffset, uint32_t value);
uint32_t pci_read(pci_address_t addr, uint8_t regOffset);
bool pci_is_connected(uint8_t bus, uint8_t device);
pci_device_info_t pci_get_device(pci_address_t addr);
pci_base_addr_reg_t pci_get_bar(pci_address_t addr, uint8_t bar);
#ifdef __cplusplus
}
#endif


#endif /* FILE_ARCH_PCI_H */