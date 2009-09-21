	ldiu %subst, bk  ; load block size (should be at most 8)
	and %0 - 1, %src_int_reg ; crop random value between 0 and bk - 1
	ldiu %2, %clobber ir0; load ir0 with this random value
	ldiu %src_float_buf[16], %dst_aux_reg   ; load a pointer to a buffer of 16 words
	addi %subst, %6
	andn %7, %6  ; align circular buffer start on block size
	ldiu %st_in, %clobber st
	%subst *%6++(ir0)%%, %src_dst_float_reg; <-- instruction under test
	ldiu st, %st_out

