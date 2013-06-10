loadRandom proc
	mov ah,0 ; Read system clock counter
	int 1Ah ; System and Real Time Clock BIOS Services
	mov	ax,dx ; low order word of tick count
	mov	dx,0
	mov al,9
	div	al ; Máximo
	mov	randomNum,dx ; Resto da Divisão
	ret
loadRandom endp