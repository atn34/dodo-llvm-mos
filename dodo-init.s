	.globl	__dodo_init
.section .init.10,"axR",@progbits
__dodo_init:
	; make sure flags are in a sane state
	cld
	cli
	; tell dodo where the software stack pointer is
	lda #__rc0
	; Init hardware stack pointer
	sta $00
	ldx #$FF
	txs
