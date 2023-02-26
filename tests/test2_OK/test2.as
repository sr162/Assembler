; file ps.as
.entry LIST
.extern W

MAIN: 	add r3, LIST
LOOP: 	prn #48
STR: 	.string "abcd"
	lea STR, r6
	inc r6
	mov r3, W
	.data -100


	sub r1, r4 
	bne END(W,	 r0)
	cmp val1, #-6
LIST: 	.data 6, -9
	bne END
	dec K
.entry MAIN
	sub LOOP ,r7
END:	stop
.entry K
K:	.data 31
.extern val1
