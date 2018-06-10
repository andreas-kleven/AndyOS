#include "filesystem.h"

IFileSystem::IFileSystem(BlockDevice* dev)
{
	this->device = dev;
}
