#pragma once

#define CALL_INTERRUPT(x) asm("int %0" ::"N"((x)))