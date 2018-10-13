;
; interrupt.s -- Contains interrupt service routine wrappers.

global idt_flush    ; Allows the C code to call idt_flush().

global isr_common_stub
global irq_common_stub
global syscall_common_stub

idt_flush:
   mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter.
   lidt [eax]        ; Load the IDT pointer.
   ret

; This macro creates a stub for an ISR which does NOT pass it's own
; error code (adds a dummy errcode byte).
%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    push byte 0                 ; Push a dummy error code.
    push %1                ; Push the interrupt number.
    jmp isr_common_stub         ; Go to our common handler code.
%endmacro

; This macro creates a stub for an ISR which passes it's own
; error code.
%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    push %1                ; Push the interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_SYSCALL 2
  global syscall_%1
  syscall_%1:
    push byte 0                 ; Push a dummy error code.
    push %2                ; Push the interrupt number.
    jmp syscall_common_stub         ; Go to our common handler code.
%endmacro

%macro IRQ 2
  global irq%1
  irq%1:
    push byte 0
    push %2
    jmp irq_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47

ISR_SYSCALL ludos, 0x70 ; ludos_syscall
ISR_SYSCALL linux, 0x80 ; linux_syscall

; In isr.c
extern syscall_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
syscall_common_stub:
    ; Store general purpose
    pushad

    ; Store segments
    push ds
    push es
    push fs
    push gs

    ; Switch to kernel segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Push stack pointer
    push esp

    ; Restore interrupts
    sti

    ; Call handler
    call syscall_handler
    ; Set stack pointer to returned value

    mov esp, eax

    ; Restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general purpose
    ;add esp, 4 ; skip stack's eax in order to keep the syscall return value
    popad

    ; Skip intr and error in Registers struct
    add esp, 8

    ; Restore rest
    iret

; In isr.c
extern isr_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    ; Store general purpose
    pushad

    ; Store segments
    push ds
    push es
    push fs
    push gs

    ; Switch to kernel segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Push stack pointer
    push esp
    ; Call handler
    call isr_handler
    ; Set stack pointer to returned value

    mov esp, eax

    ; Restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general purpose
    popad

    ; Skip intr and error in Registers struct
    add esp, 8

    ; Restore rest
    iret

extern irq_handler

; This is our common IRQ stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
irq_common_stub:
    ; Store general purpose
    pushad

    ; Store segments
    push ds
    push es
    push fs
    push gs

    ; Switch to kernel segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Push stack pointer
    push esp

    ; Call handler
    call irq_handler
    ; Set stack pointer to returned value
    mov esp, eax

    ; Restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general purpose
    popad

    ; Skip intr and error in Registers struct
    add esp, 8

    ; Restore rest
    iret
