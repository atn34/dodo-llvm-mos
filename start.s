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
	.section	".note.GNU-stack","",@progbits
	.addrsig
