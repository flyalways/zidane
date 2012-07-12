	NAME UTIL

?PR?_fat16_free_cluster_offset?UTIL		SEGMENT CODE
?PR?_fat32_free_cluster_offset?UTIL		SEGMENT CODE
?PR?_count_free_cluster_in_fat16_sector?UTIL	SEGMENT CODE
?PR?_count_free_cluster_in_fat32_sector?UTIL	SEGMENT CODE
PUBLIC _fat16_free_cluster_offset
PUBLIC _fat32_free_cluster_offset
PUBLIC _count_free_cluster_in_fat16_sector
PUBLIC _count_free_cluster_in_fat32_sector

/*! \internal
 *	\param	p		Buffer for FAT.
 *	\param	size	Should be equal to or less than 128.
 *	\return	Free cluster entry offset in the sector.
 */
;U8 fat16_free_cluster_offset(U8 xdata * p, U8 size)
;{
;	UBYTE count = size;
;	while (count)
;	{
;		if (*p++ == 0)
;		{
;			if (*p++ == 0)
;			{
;				return size-count;
;			}
;		}
;		else
;			p++;
;		count--;
;	}
;	return size;
;}
	RSEG  ?PR?_fat16_free_cluster_offset?UTIL
_fat16_free_cluster_offset:
	;// size = R5
	;// p = {R6, R7}
	mov		DPH, R2;R6
	mov		DPL, R1;R7
	mov	A, R5
	mov	R7, A			;// R7 = size, R5 = count
	jz	?__fat16_search_free_loop_end
?__fat16_search_free_loop_begin:
	movx	A, @DPTR
	inc	DPTR
	jnz	?__fat16_search_not_free_b0
	movx	A, @DPTR
	inc	DPTR
	jnz	?__fat16_search_not_free_b1
	clr	C
	mov	A, R7
	subb	A, R5			;// A = size - count
	mov	R7, A
	ret
?__fat16_search_not_free_b0:
	inc	DPTR
?__fat16_search_not_free_b1:
?__fat16_search_free_loop_condition:
	djnz	R5, ?__fat16_search_free_loop_begin	;// count--
?__fat16_search_free_loop_end:
	ret						;// return R7 = size


;! \internal Search FAT32 free clusters in the given buffer p[].
;	\param	p		Buffer for FAT.
;	\param	size	Should be equal to or less than 128.
;	\return	Free cluster entry offset in the sector.
; U8 fat32_free_cluster_offset(U8 xdata * p, U8 size)
;{
;	UBYTE offset;
;	offset = 0;
;	while (size)
;	{
;		if (*p++ == 0)
;		{
;			if (*p++ == 0)
;			{
;				if (*p++ == 0)
;				{
;					if (*p++ == 0)
;					{
;						return offset;
;					}
;				}
;				else
;					p++;
;			}
;			else
;			{
;				p++;
;				p++;
;			}
;		}
;		else
;		{
;			p ++;
;			p ++;
;			p ++;
;		}
;		offset++;
;		size--;
;	}
;	return offset;
;}
	RSEG  ?PR?_fat32_free_cluster_offset?UTIL
_fat32_free_cluster_offset:
;	// size = R5
;	// p = {R6, R7}
	mov		DPH, R2;R6
	mov		DPL, R1;R7
	mov	A, R5
	mov	R7, A			// R7 = size, R5 = count
	jz	?__fat32_search_free_loop_end
?__fat32_search_free_loop_begin:
	movx	A, @DPTR
	inc	DPTR
	jnz	?__fat32_search_not_free_b0
	movx	A, @DPTR
	inc	DPTR
	jnz	?__fat32_search_not_free_b1
	movx	A, @DPTR
	inc	DPTR
	jnz	?__fat32_search_not_free_b2
	movx	A, @DPTR
	inc	DPTR
	jnz	?__fat32_search_not_free_b3
	clr	C
	mov	A, R7
	subb	A, R5			;// A = size - count
	mov	R7, A
	ret
?__fat32_search_not_free_b0:
	inc	DPTR
?__fat32_search_not_free_b1:
	inc	DPTR
?__fat32_search_not_free_b2:
	inc	DPTR
?__fat32_search_not_free_b3:
?__fat32_search_free_loop_condition:
	djnz	R5, ?__fat32_search_free_loop_begin	;// count--
?__fat32_search_free_loop_end:
	ret						;// return R7 = size


; ! \internal Count FAT16 free clusters in a given buffer p[].
;	This function is limied by the max number of argument 'size'.
;  So one call cannot search the entire 512-byte (256-cluster-entry) buffer.
;	\param	p		Buffer for FAT.
;	\param	size	Size of the buffer p[]. 0<=size<=255.
;	\return	Number of free clusters.
;U8 count_free_cluster_in_fat16_sector(U8 xdata * p, U8 size)
;{
;	UBYTE xdata * base = p;
;	UBYTE free_cluster_count = 0;
;	while (size)
;	{
;		if (*p++ == 0)
;		{
;			if (*p == 0)
;			{
;				free_cluster_count++;
;			}
;		}
;		p = (base += 2);
;		size--;
;	}
;	return free_cluster_count;
;}
	RSEG  ?PR?_count_free_cluster_in_fat16_sector?UTIL
_count_free_cluster_in_fat16_sector:
;	// size = R5
;	// p = {R6, R7}
	mov		DPL, R7
	mov		DPH, R2;R6
	clr		A
	mov		R7, A			;// R7 = free_cluster_count = 0;
	mov		A, R5
	jz		?__fat16_count_free_loop_end
?__fat16_count_free_loop_begin:
	movx	A, @DPTR
	inc		DPTR
	jnz		?__fat16_count_not_free_b0
	movx	A, @DPTR
	inc		DPTR
	jnz		?__fat16_count_not_free_b1

	inc		R7					;// free_cluster_count++
	djnz	R5, ?__fat16_count_free_loop_begin
	jmp		?__fat16_count_free_loop_end
?__fat16_count_not_free_b0:
	inc		DPTR
?__fat16_count_not_free_b1:
?__fat16_count_free_loop_condition:
	djnz	R5, ?__fat16_count_free_loop_begin
?__fat16_count_free_loop_end:

	ret


;! \internal
;! \internal Count FAT32 free clusters in a given buffer p[].
;	\param	p		Buffer for FAT.
;	\param	size	Size of the buffer p[].
;	\return	Number of free clusters.
; U8 count_free_cluster_in_fat32_sector(U8 xdata * p, U8 size)
;{
;	UBYTE xdata * base = p;
;	UBYTE free_cluster_count = 0;
;	while (size)
;	{
;		if (*p++ == 0)
;		{
;			if (*p++ == 0)
;			{
;				if (*p++ == 0)
;				{
;					if (*p == 0)
;					{
;						free_cluster_count++;
;					}
;				}
;			}
;		}
;		p = (base += 4);
;		size--;
;	}
;	return free_cluster_count;
;}
	RSEG  ?PR?_count_free_cluster_in_fat32_sector?UTIL
_count_free_cluster_in_fat32_sector:
;	 size = R5
;	 p = {R6, R7}
	mov		DPH, R6
	mov		DPL, R1;R7
	clr		A
	mov		R7, A			;// R7 = free_cluster_count = 0;
	mov		A, R5
	jz		?__fat32_count_free_loop_end
?__fat32_count_free_loop_begin:
	movx	A, @DPTR
	inc		DPTR
	jnz		?__fat32_count_not_free_b0
	movx	A, @DPTR
	inc		DPTR
	jnz		?__fat32_count_not_free_b1
	movx	A, @DPTR
	inc		DPTR
	jnz		?__fat32_count_not_free_b2
	movx	A, @DPTR
	inc		DPTR
	jnz		?__fat32_count_not_free_b3

	inc		R7					;// free_cluster_count++
	djnz	R5, ?__fat32_count_free_loop_begin
	jmp		?__fat32_count_free_loop_end
?__fat32_count_not_free_b0:
	inc		DPTR
?__fat32_count_not_free_b1:
	inc		DPTR
?__fat32_count_not_free_b2:
	inc		DPTR
?__fat32_count_not_free_b3:
?__fat32_count_free_loop_condition:
	djnz	R5, ?__fat32_count_free_loop_begin
?__fat32_count_free_loop_end:

	ret

	END

