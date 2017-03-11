	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 11
	.globl	__strlen
	.align	4, 0x90
__strlen:                               ## @_strlen
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$1088, %rsp             ## imm = 0x440
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	%rdi, -1048(%rbp)
	cmpq	$0, -1048(%rbp)
	jne	LBB0_2
## BB#1:
	leaq	L_.str(%rip), %rsi
	leaq	L___func__._strlen(%rip), %rdx
	leaq	L_.str.1(%rip), %rcx
	movl	$7, %r8d
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	movb	$0, %al
	callq	_fprintf
	leaq	-1040(%rbp), %rdi
	movl	-1052(%rbp), %esi
	movl	%eax, -1060(%rbp)       ## 4-byte Spill
	callq	_backtrace
	leaq	L_.str.2(%rip), %rsi
	movq	___stderrp@GOTPCREL(%rip), %rcx
	movl	%eax, -1052(%rbp)
	movq	(%rcx), %rdi
	movl	-1052(%rbp), %edx
	movb	$0, %al
	callq	_fprintf
	movq	___stderrp@GOTPCREL(%rip), %rcx
	leaq	-1040(%rbp), %rdi
	movl	-1052(%rbp), %esi
	movq	(%rcx), %rcx
	movq	%rdi, -1072(%rbp)       ## 8-byte Spill
	movq	%rcx, %rdi
	movl	%eax, -1076(%rbp)       ## 4-byte Spill
	movl	%esi, -1080(%rbp)       ## 4-byte Spill
	callq	_fileno
	movq	-1072(%rbp), %rdi       ## 8-byte Reload
	movl	-1080(%rbp), %esi       ## 4-byte Reload
	movl	%eax, %edx
	callq	_backtrace_symbols_fd
	leaq	L_.str.3(%rip), %rsi
	movq	___stderrp@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rdi
	movb	$0, %al
	callq	_fprintf
	movl	$7, %edi
	movl	%eax, -1084(%rbp)       ## 4-byte Spill
	callq	_exit
LBB0_2:
	movl	$0, -1056(%rbp)
LBB0_3:                                 ## =>This Inner Loop Header: Depth=1
	movq	-1048(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -1048(%rbp)
	cmpb	$0, (%rax)
	je	LBB0_5
## BB#4:                                ##   in Loop: Header=BB0_3 Depth=1
	movl	-1056(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -1056(%rbp)
	jmp	LBB0_3
LBB0_5:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movl	-1056(%rbp), %ecx
	movq	(%rax), %rax
	cmpq	-8(%rbp), %rax
	movl	%ecx, -1088(%rbp)       ## 4-byte Spill
	jne	LBB0_7
## BB#6:
	movl	-1088(%rbp), %eax       ## 4-byte Reload
	addq	$1088, %rsp             ## imm = 0x440
	popq	%rbp
	retq
LBB0_7:
	callq	___stack_chk_fail
	.cfi_endproc

	.globl	_thunk
	.align	4, 0x90
_thunk:                                 ## @thunk
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp5:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	xorl	%eax, %eax
	movl	%eax, %edi
	callq	__strlen
	movl	%eax, -4(%rbp)          ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_main
	.align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp6:
	.cfi_def_cfa_offset 16
Ltmp7:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp8:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	callq	_thunk
	callq	_thunk
	callq	_thunk
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"ASSERTION FAILED: _strlen(NULL) is undefined (`s != NULL` was false, in %s(), at %s:%i)\n"

L___func__._strlen:                     ## @__func__._strlen
	.asciz	"_strlen"

L_.str.1:                               ## @.str.1
	.asciz	"modern.c"

L_.str.2:                               ## @.str.2
	.asciz	"%i frames\n"

L_.str.3:                               ## @.str.3
	.asciz	"done\n"


.subsections_via_symbols
