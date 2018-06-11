#pragma once

void* operator new(unsigned size);
void operator delete(void* p);
void operator delete(void* p, unsigned size);
void* operator new[](unsigned size);
void operator delete[](void* p);