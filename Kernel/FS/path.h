#pragma once

class Path
{
private:
	char* buf;

public:
	int count;
	char** parts;

	Path();
	Path(const char* path);
	~Path();

	Path Parent() const;

	bool operator==(const Path& path) const;
};