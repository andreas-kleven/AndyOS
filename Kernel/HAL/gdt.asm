section .text
bits 32

; gdt loading part of start.asm
global _gdt_flush    ; Allows the C code to link to this
;extern _gp            ; Says that '_gp' is in another file
_gdt_flush:
  ;lgdt [_gp]        ; Load the GDT with our '_gp' which is a special pointer
  mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  jmp 0x08:flush2  ; 0x08 is the offset to our code segment: Far jump!
flush2:
  ret              ; Returns back to the C code!