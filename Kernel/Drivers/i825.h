#pragma once
#include "definitions.h"
#include "PCI/pci.h"

static class i825
{
public:
	static STATUS Init(PCI_DEVICE* dev);
};
