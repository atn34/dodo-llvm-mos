	.text
	.globl	_start                          ; -- Begin function _start
	.type	_start,@function
_start:                                 ; @_start
; %bb.0:
	cld
	cli
	lda #mos16lo(__stack)
	sta mos8(__rc0)
	lda #mos16hi(__stack)
	sta mos8(__rc1)
	lda #__rc0
	sta $00
	ldx #$FF
	txs
	jsr zero_bss
	jsr main
.L1:
	jmp .L1
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start
                                        ; -- End function
	.section	".note.GNU-stack","",@progbits
	.addrsig
