	.text
	.globl	_start                          ; -- Begin function _start
	.type	_start,@function
_start:                                 ; @_start
; %bb.0:
	sei
	cld
	clv
	ldx #$FF
	txs
	cli
	jsr _zero_bss
	jsr main
.L1:
	jmp .L1
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start
                                        ; -- End function
	.ident	"clang version 15.0.0 (https://www.github.com/llvm-mos/llvm-mos.git e08278f771d06cdd667e58ebf5b18f4c5066a250)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
