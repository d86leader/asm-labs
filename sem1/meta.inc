%ifndef ASM_LAB_META
%define ASM_LAB_META

;; vim: syntax=asm


;; a set of macros to quickly put arguments for fastcall
; putargs {{{

%macro putargs 1
	mov rdi, %1
%endmacro

%macro putargs 2
	mov rdi, %1
	mov rsi, %2
%endmacro

%macro putargs 3
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
%endmacro

%macro putargs 4
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov rcx, %4
%endmacro

%macro putargs 5
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov rcx, %4
	mov r8, %5
%endmacro

%macro putargs 6
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov rcx, %4
	mov r8, %5
	mov r9, %6
%endmacro

;; }}}


;; set of macros to push registers
; push/pop regs {{{
%macro push_regs 1-*
    %rep %0
        push %1
        %rotate 1
    %endrep
%endmacro
%macro pop_regs 1-*
    %rep %0
        %rotate -1
        pop %1
    %endrep
%endmacro
%macro save_all 0
    push_regs rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11
%endmacro
%macro restore_all 0
    pop_regs rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11
%endmacro
; }}}


;; macros to put arguments for syscalls
; syscallargs {{{

%macro syscallargs 1
	mov rdi, %1
%endmacro

%macro syscallargs 2
	mov rdi, %1
	mov rsi, %2
%endmacro

%macro syscallargs 3
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
%endmacro

%macro syscallargs 4
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov r10, %4
%endmacro

%macro syscallargs 5
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov r10, %4
	mov r8,  %5
%endmacro

%macro syscallargs 6
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov r10, %4
	mov r8,  %5
	mov r9,  %6
%endmacro

;; }}}


;; macros to call functions and syscalls
; calls {{{

%macro sys_call 2+
	mov rax, %1
	syscallargs %2
	syscall
%endmacro


%macro vcall 2+
	putargs %2
	call %1
%endmacro

; }}}


;; macros to define functions with named parameters
; defun {{{

;; define subroutine: function without parameters and stackframe
%macro defsub 1
	%push %1
    %1:
%endmacro
%macro endsub 0
    ret
	%pop
%endmacro

%macro defun 2
	%push %1
	%define %2 rdi
     %1:
	push rbp
	mov rbp, rsp
%endmacro

%macro defun 3
	%push %1
	%define %2 rdi
	%define %3 rsi
     %1:
	push rbp
	mov rbp, rsp
%endmacro

%macro defun 4
	%push %1
	%assign %2 rdi
	%assign %3 rsi
	%assign %4 rdx
     %1:
	push rbp
	mov rbp, rsp
%endmacro

%macro defun 5
	%push %1
	%assign %2 rdi
	%assign %3 rsi
	%assign %4 rdx
	%assign %5 rcx
     %1:
	push rbp
	mov rbp, rsp
%endmacro

%macro defun 6
	%push %1
	%assign %2 rdi
	%assign %3 rsi
	%assign %4 rdx
	%assign %5 rcx
	%assign %6 r8
     %1:
	push rbp
	mov rbp, rsp
%endmacro

%macro defun 7
	%push %1
	%assign %2 rdi
	%assign %3 rsi
	%assign %4 rdx
	%assign %5 rcx
	%assign %6 r8
	%assign %7 r9
     %1:
	push rbp
	mov rbp, rsp
%endmacro

; }}}


;; ending function definition
; endfun {{{
%macro endfun 0
	%pop
%endmacro
; }}}


;; returning from function
; return {{{

%macro return 0
    mov rsp, rbp
	pop rbp
	ret
%endmacro

%macro return 1
	mov rax, %1
	mov rsp, rbp
	pop rbp
	ret
%endmacro

; }}}


;; conditional expression
; if-else-end {{{
%macro if 3
    %push if_context
    cmp   %1, %3
    j%-2  %$if_jumpout
%endmacro

%macro andif 3
    %ifctx if_context
        cmp  %1, %3
        j%-2 %$if_jumpout
    %else
        %error "using andif without if"
    %endif
%endmacro

%macro orif 3
    %ifctx if_context
        %repl orif_context
        jmp %$orif_holds
       %$if_jumpout:
        cmp %1, %3
        j%+2 %$orif_holds
    %elifctx orif_context
        cmp %1, %3
        j%+2 %$orif_holds
    %else
        error "using orif without if"
    %endif
%endmacro

%macro then 0
    %ifctx if_context
        ;; pass
    %elifctx orif_context
        jmp %$if_end
       %$orif_holds:
    %else
        %error "can only use then with if/andif/orif"
    %endif
%endmacro

%macro elseif 3
    %ifctx if_context
       %$if_jumpout:
        %pop
        if %1, %2, %3
    %else
        %error "elseif without corresponding if (or elseif with orif/andif)"
    %endif
%endmacro

%macro else 0
    %ifctx if_context
        %repl else_context
        jmp   %$if_end
       %$if_jumpout:
    %else
	    %error "else without corresponding if (or else with orif/andif)"
    %endif
%endmacro

%macro endif 0
    %ifctx if_context
        %$if_jumpout:
        %pop
    %elifctx else_context
        %$if_end:
        %pop
    %elifctx orif_context
        %$if_end:
        %pop
    %else
        %error "endif without corresponding if"
    %endif
%endmacro
; }}}


;; easily print const string
; puts/die {{{
%macro puts 1+
    jmp %%endstr
   %%str: db %1
   %%endstr:
    sys_write 1, %%str, %%endstr - %%str
%endmacro

%macro die 1+
    sys_write 0, %%str, %%endstr - %%str
    sys_exit 1
    %%str: db %1
	%%endstr:
%endmacro
; }}}


;; define const string in text section
; defstr {{{
%macro defstr 2+
    jmp %%end
   %1: db %2
   %%end:
%endmacro
; }}}

%endif
