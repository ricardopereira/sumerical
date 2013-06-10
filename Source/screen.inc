cleanScreen proc
	xor bx,bx
	mov ah,' '
	mov al,10000100b ; Cinza (1000) e Vermelho (0100)
	mov cx,25*80
doLoop:   
	mov es:[bx],ah
	inc bx
	mov es:[bx],al
	inc bx
	loop doLoop
	ret
cleanScreen endp

setCursor macro col,row
	push ax
	push bx
	push dx
	mov	ah,02h
	mov	bh,0
	mov	dl,col
	mov	dh,row
	int	10h
	pop dx
	pop bx
	pop ax
endm

getCursor proc
	push ax
	push bx
	push dx
	mov	ah,3h
	mov bh,1d
	int	10h
	mov	activeCol,dl
	mov	activeRow,dh
	pop dx
	pop bx
	pop ax
	ret
getCursor endp

print macro str
	push ax
	push dx
	mov	ah,09h
	lea	dx,str
	int	21h
	pop dx
	pop ax
endm

printC macro char
	push ax
	push dx
	mov ah,02h
	mov	dl,char
	int	21h
	pop dx
	pop ax
endm

printD macro digito
	push ax
	push dx
	mov ah,02h
	mov	dl,digito
	add dl,30h
	int	21h
	pop dx
	pop ax
endm

printNum macro num
	push ax
	push bx
	push dx
	xor bp,bp
	
	xor ax,ax
	mov bl,10d
	
	;Segundo digito em ah
	mov al,num
	div bl
	inc bp
	add ah,30h
	mov numInStr[bp],ah
	;Primeiro digito em ah
	mov ah,0
	div bl
	dec bp
	add ah,30h
	mov numInStr[bp],ah
	
	print numInStr
	pop dx
	pop bx
	pop ax
endm

printWord macro num
	push ax
	push bx
	push cx
	push dx
	push si
	push di
	
	cmp num,0
	je doEmpty

	mov ax,num ;Valor
	mov bx,10d
	xor si,si
	xor dx,dx
	mov string[0],'$'
doWhile:
	cmp ax,0
	je doPrintString

	xor dx,dx
	div bx
	add dl,30h
	inc si

	mov di,si
doOrder:
	cmp di,0
	je doNext
	mov cl,string[di-1]
	mov string[di],cl
	dec di
	jmp doOrder
doNext:
	mov string[0],dl
	jmp doWhile
doEmpty:
	mov string[0],'0'
	mov string[1],'$'
doPrintString:
	print string
	
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
endm

waitKeyboard proc
	mov keyPressed,0
doWhile:
	mov ah,0Bh ;Keyboard Buffer
	int 21h
	cmp al,0FFh ;Empty
	jne doWhile
	
	mov ah,08h ;Read keyboard buffer
	int 21h
	
	cmp al,0 ;Extended key?
	jne doReadKey
	mov ah,08h ;Extended
	int 21h
doReadKey:
	mov keyPressed,al
	ret
waitKeyboard endp

readScreen proc
	mov ah,3Dh ; Open file
	mov al,00 ; Read only
	int 21h
	jc errorOpen
	mov fileHandle,ax ; File content
	
forEach:
	mov ah,3Fh ; Read from file
	mov bx,fileHandle
	mov cx,1 ; Number of bytes to read
	lea dx,fileBf
	int 21h
	jc errorRead
	cmp	ax,0 ; Check EOF - number of bytes read
	je closeFile
	
	mov ah,02h ; Print output Bf
	mov	dl,fileBf
	int	21h
	jmp	forEach
	
errorOpen:
	print msgErrorOpen
	jmp return

errorRead:
	print msgErrorRead
	
closeFile:
	mov ah,3Eh ; Close file
	mov bx,fileHandle
	int 21h
	jnc return
	print msgErrorClose
	
return:
	ret
readScreen endp