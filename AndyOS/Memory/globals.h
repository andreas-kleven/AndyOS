#pragma once

void* operator new(unsigned size);
void operator delete(void* p);
void* operator new[](unsigned size);
void operator delete[](void* p);