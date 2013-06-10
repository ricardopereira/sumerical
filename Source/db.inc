readGame proc
	mov ah,3Dh ; Open file
	mov al,00 ; Read only
	int 21h
	jc errorOpen
	;Init
	mov fileHandle,ax ; File content
	mov fileLine,0
	mov fileGameIdx,0
	mov tabGameLast,0
	mov tabGameSize,0
	mov tabGameTotalSize,0
	mov totalGames,0
	;Tabuleiro por defeito
	mov al,tabGameSizeDefault
	mov tabGameSize,al ; Default e Maximo
	;Indice para o tabuleiro
	xor si,si
	;Selecao de jogo
	mov al,loadGame
	mov ah,3d ; Cada jogo sao 3 linhas no ficheiro
	mul ah ; Multiplica por 3 para saber quais os indices do jogo a carregar
	;Primeiro indice: ID do Jogo (0-Default = Jogo original)
	inc al
	mov lineGameID,al
	;Segundo indice: Tamanho do Tabuleiro (0-Default = 10)
	inc al
	mov lineGameTabSize,al
	;Terceiro indice: Tabuleiro de Jogo
	inc al
	mov lineGameTab,al
	
forEach:
	mov ah,3Fh ; Read from file
	mov bx,fileHandle
	mov cx,1 ; Number of bytes to read
	lea dx,fileBf
	int 21h
	jc errorRead
	cmp	ax,0 ; Check EOF - number of bytes read
	je closeFile
	
	;Index por linha
	cmp fileBf,10d ; New Line
	je doIncLine
	
	;Primeira linha reservada para o total
	;de jogos existentes no ficheiro
	cmp fileLine,0
	je doTotalGames
	;ID do Jogo
	mov al,fileLine
	cmp al,lineGameID
	je doCheckGameID
	;Tamanho do Tabuleiro
	mov al,fileLine
	cmp al,lineGameTabSize
	je doCheckGameSize
	
	jmp doGameLine
	
doIncLine:
	inc fileLine
	jmp forEach
	
doTotalGames:
	;Verifiar inteiro
	cmp fileBf,'0'
	jb forEach
	cmp fileBf,'9'
	ja forEach
	;Total de Jogos
	mov ah,fileBf
	sub ah,30h
	cmp ah,0
	je errorFileFormat
	mov totalGames,ah
	jmp forEach
	
doCheckGameID:
	;Verifiar inteiro
	cmp fileBf,'0'
	jb forEach
	cmp fileBf,'9'
	ja forEach
	;Verificar o ID de Jogo
	mov ah,fileBf
	sub ah,30h
	cmp ah,loadGame
	jne errorFileFormat
	jmp forEach
	
doCheckGameSize:
	;Verifiar inteiro
	cmp fileBf,'0'
	jb forEach
	cmp fileBf,'9'
	ja forEach
	;Tamanho do Tabuleiro
	mov ah,fileBf
	sub ah,30h
	cmp ah,tabGameSizeDefault
	ja errorFileFormat
	cmp ah,0
	je doDefaultGameSize
	mov tabGameSize,ah
	mov al,tabGameSize
	mov bl,tabGameSize
	mul bl
	mov tabGameTotalSize,al
	jmp forEach
	
doDefaultGameSize:
	mov ah,tabGameSizeDefault
	mov tabGameSize,ah
	mov al,tabGameSize
	mov bl,tabGameSize
	mul bl
	mov tabGameTotalSize,al
	jmp forEach

doGameLine:
	mov al,fileLine
	cmp al,lineGameID
	jb forEach
	cmp al,lineGameTab
	ja closeFile

	;Verifiar inteiro
	cmp fileBf,'0'
	jb forEach
	cmp fileBf,'9'
	ja forEach
	
	inc fileGameIdx
	;Entre '0' e '9'
	mov ah,fileBf
	sub ah,30h
	mov tabGame[si],ah
	mov tabGameLast,si
	inc si
	jmp forEach
	
errorOpen:
	print msgErrorOpen
	jmp return
	
errorFileFormat:
	print msgErrorFileFormat
	jmp closeFile

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
readGame endp

saveGame proc
	mov ah,3Dh ; Open file
	mov al,01 ; Write only
	int 21h
	jc errorOpen
	;JOGO POR DEFEITO (ID:0) tem que ter sempre 10 celulas
	
	mov bx,fileHandle
	mov cx,1 ; Number of bytes to read
	lea dx,fileBf
	;Total de Jogos
	mov ah,40h ; Write to file
	mov al,totalGames
	add al,1
	add al,30h
	mov fileBf,al
	int 21h
	jc errorWrite
	
	;Posicionar
	mov ah,42h ; Seek
	mov al,02 ; End of file plus offset  (SEEK_SET)
	xor cx,cx
	xor dx,dx
	int 21h
	jc errorFileFormat
	
	mov cx,1 ; Number of bytes to read
	lea dx,fileBf
	
	;Quebra de linha
	mov ah,40h ; Write to file
	mov fileBf,10d
	int 21h
	
	;ID de Jogo
	mov ah,40h ; Write to file
	mov al,totalGames
	add al,30h
	mov fileBf,al
	int 21h
	
	;Quebra de linha
	mov ah,40h ; Write to file
	mov fileBf,10d
	int 21h
	
	;Tamanho do Tabuleiro
	mov ah,40h ; Write to file
	mov al,tabGameSize
	cmp al,tabGameSizeDefault
	jne doGameSize
	mov al,0
	doGameSize:
	add al,30h
	mov fileBf,al
	int 21h
	
	;Quebra de linha
	mov ah,40h ; Write to file
	mov fileBf,10d
	int 21h
	
	;Numeros do tabuleiro
	xor si,si
	;Contar total do tabuleiro
	xor dx,dx
	mov dl,tabGameTotalSize
	mov Count,dx
	lea dx,fileBf
doGameCount:
	cmp si,Count ;Total
	je doGameCountFinish

	;Grava valor da celula
	mov ah,40h ; Write to file
	mov al,tabGame[si]
	add al,30h
	mov fileBf,al
	int 21h
	;Espacamento
	mov ah,40h ; Write to file
	mov fileBf,' '
	int 21h

	inc si
	jmp doGameCount
doGameCountFinish:
	inc totalGames
	jmp closeFile
	
errorOpen:
	print msgErrorOpen
	jmp return
	
errorFileFormat:
	print msgErrorFileFormat
	jmp closeFile

errorWrite:
	print msgErrorWrite
	
closeFile:
	mov ah,3Eh ; Close file
	mov bx,fileHandle
	int 21h
	jnc return
	print msgErrorClose
return:
	ret
saveGame endp