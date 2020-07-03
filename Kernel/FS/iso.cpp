#include "iso.h"
#include "string.h"
#include "vfs.h"
#include "Drivers/driver.h"
#include "Lib/debug.h"

int IsoFS::Mount(BlockDriver *driver, DENTRY *root_dentry)
{
	this->driver = driver;

	desc = (ISO_PRIMARY_DESC *)(new char[0x1000]);
	if (!ReadBlock(0x10, desc, 0x1000))
		return -1;

	ISO_DIRECTORY *table = &desc->rootDirectory;

	root = (ISO_DIRECTORY *)(new char[table->filesize_LSB]);
	if (!ReadBlock(table->location_LSB, root, table->filesize_LSB))
		return -1;

	root_dentry->inode = GetInode(root, root_dentry);
	return 0;
}

int IsoFS::GetChildren(DENTRY *parent, const char *find_name)
{
	INODE *inode = parent->inode;
	ISO_DIRECTORY *dir = root;

	if (inode->ino > 1)
	{
		dir = (ISO_DIRECTORY *)(new char[inode->size]);
		if (!ReadBlock(inode->ino, dir, inode->size))
			return -1;
	}

	while (dir && dir->length)
	{
		char name[32];
		GetName(dir, name);

		if (!find_name || strcmp(name, find_name) == 0)
		{
			DENTRY *dentry = VFS::AllocDentry(parent, name);

			if (!dentry->inode)
			{
				FillDentry(dir, dentry);
				dentry->inode = GetInode(dir, dentry);
				VFS::AddDentry(parent, dentry);
			}

			if (find_name)
				break;
		}

		dir = (ISO_DIRECTORY *)((size_t)dir + dir->length);
		size_t space_left = ISO_SECTOR_SIZE - (size_t)dir % ISO_SECTOR_SIZE;

		if (space_left < sizeof(ISO_DIRECTORY))
			dir = (ISO_DIRECTORY *)((size_t)dir + space_left);
	}

	return 0;
}

int IsoFS::Read(FILE *file, void *buf, size_t size)
{
	return driver->Read(file->pos + file->dentry->inode->ino * ISO_SECTOR_SIZE, buf, size);
}

void IsoFS::GetName(ISO_DIRECTORY *dir, char *buf)
{
	memcpy(buf, &dir->identifier, dir->idLength);
	buf[dir->idLength] = 0;
	stolower(buf);

	int length = strlen(buf);

	if (length >= 2)
	{
		if (buf[length - 2] == ';')
			buf[length - 2] = 0;
	}

	if (length >= 3)
	{
		if (buf[length - 3] == '.')
			buf[length - 3] = 0;
	}
}

int GetType(ISO_DIRECTORY *dir)
{
	int flags = 0;

	if (dir->flags & ISO_FLAG_DIRECTORY)
		flags |= INODE_TYPE_DIRECTORY;
	else
		flags |= INODE_TYPE_REGULAR;

	return flags;
}

INODE *IsoFS::GetInode(ISO_DIRECTORY *dir, DENTRY *dentry)
{
	INODE *inode = VFS::AllocInode(dentry);
	inode->ino = dir->location_LSB;
	inode->size = dir->filesize_LSB;
	inode->type = GetType(dir);

	if (dir == root)
		inode->ino = 1;

	return inode;
}

void IsoFS::FillDentry(ISO_DIRECTORY *dir, DENTRY *dentry)
{
	dentry->type = GetType(dir);
}

int IsoFS::ReadBlock(int block, void *buf, size_t size)
{
	return driver->Read(block * ISO_SECTOR_SIZE, buf, size);
}
