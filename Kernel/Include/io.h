#pragma once
#include "types.h"

uint8 mmio_read8(uint32 addr);
uint16 mmio_read16(uint32 addr);
uint32 mmio_read32(uint32 addr);

void mmio_write8(uint32 addr, uint8 val);
void mmio_write16(uint32 addr, uint16 val);
void mmio_write32(uint32 addr, uint32 val);