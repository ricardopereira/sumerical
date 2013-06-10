loadRandomGameValue proc
	push ax
	push cx
	push dx
	mov ah,0
	int 1Ah ;System Clock
	mov	ax,dx ;dx fica com o contador de ticks (0-18206)
	mov	dx,0000h
	xor cx,cx
	mov cl,maxGameValue ;valor máximo do numero aleatorio
	div	cx
	cmp dl,2
	jb doNotValid
	mov	gameValue,dl ;número gerado
	jmp doFinish
doNotValid:
	mov	gameValue,2
doFinish:
	pop dx
	pop cx
	pop ax
	ret
loadRandomGameValue endp

loadRandomNum proc
	push ax
	push cx
	push dx
	mov ah,0
	int 1Ah ;System Clock
	mov	ax,dx ;dx fica com o contador de ticks (0-18206)
	mov	dx,0000h
	xor cx,cx
	mov cl,9 ;valor máximo do numero aleatorio
	div	cx
	cmp dl,1
	jb doNotValid
	mov	randomNum,dl ;número gerado
	jmp doFinish
doNotValid:
	mov	randomNum,1
doFinish:
	pop dx
	pop cx
	pop ax
	ret
loadRandomNum endp