buildTabuleiro proc
	mov cellIdx,0
	xor bx,bx
	;Indice para tabuleiro
	xor si,si
	
	;Posicionar linha
	mov al,videoRow
	mov dl,160d
	mul dl
	mov bx,ax
	;Posicionar coluna
	mov al,videoCol
	mov dl,2
	mul dl
	add bx,ax
	
	;Init
	mov par,0
	mov videoRow,0
	mov al,tabGameSize
	mov dl,2
	mul dl
	mov dl,al
	inc dl
	mov videoLastCell,dl
	
	;Formato do tabuleiro
	mov	ah,219d ; Caracter

doWhile:
	;Verificar se chegou ao fim
	mov dl,videoRow
	cmp dl,videoLastCell
	je doFinish
	
	;Verificar que tipo de linha a criar
	cmp par,0
	je doPrintEntireLine
	jmp doPrintCellLine

	;Print linha de divisao
doPrintEntireLine:
	xor cx,cx
	mov cl,videoLastCell; videoCol
	mov par,1
	mov	al,00000100b ; Formatacao (Vermelho)
doLoopEntireLine:
	mov	es:[bx],ah ; ASCII
	mov	es:[bx+1],al ; Formatacao
	add bx,2d ;Next Col
	loop doLoopEntireLine
	jmp doNextRow
	
	;Print linha com celulas
doPrintCellLine:
	xor cx,cx
	mov cl,videoLastCell; videoCol
	mov par,0
	mov dl,0
doLoopCellLine:
	cmp dl,1
	je doPrintCellNumber
	mov	es:[bx],ah ; ASCII
	mov	al,00000100b ; Formatacao (Vermelho)
	mov dl,1
	jmp doContinue
doPrintCellNumber:
	mov dh,tabGame[si]
	add dh,30h
	mov	es:[bx],dh ; ASCII

	cmp cellMultiColor,1
	je doPrintWithColor
	mov	al,00000111b ; Formatacao (Branco)
	jmp doIncrementNumber
doPrintWithColor:
	mov	al,tabGame[si] ; Formatacao
doIncrementNumber:
	inc si
	mov dl,0
doContinue:
	mov	es:[bx+1],al ; Formatacao
	add bx,2d ;Next Col
	loop doLoopCellLine
	jmp doNextRow
	
	;Proxima linha
doNextRow:
	inc videoRow
	;inc al ;Nova cor
	xor dx,dx
	mov dl,videoLastCell
	;Posicionar a coluna
	sub bx,dx
	sub bx,dx
	add bx,160d ;Next Row
	jmp doWhile

doFinish:
	ret
buildTabuleiro endp

startGame proc
	;Construir tabuleiro
	mov videoCol,10d
	mov videoRow,2d
	call buildTabuleiro
	
	;Init
	mov al,maxSeconds
	mov seconds,al
	mov selectedValue,0
	mov selectedCellCount,0
	mov gamePoints,0
	mov gameTotalValue,0
	mov cellIdx,0
	mov isGameOver,0
	;Limite superior e da direita do tabuleiro
	mov startTabCol,11d
	mov startTabRow,3d

	;Limite da esquerda do tabuleiro
	mov al,2
	mov bl,tabGameSize
	dec bl
	mul bl
	mov bl,startTabCol
	add bl,al
	mov endTabCol,bl
	;Limite inferior do tabuleiro
	mov al,2
	mov bl,tabGameSize
	dec bl
	mul bl
	mov bl,startTabRow
	add bl,al
	mov endTabRow,bl
	
	;Coloca valor total a atingir
	mov cl,timeRow
	add cl,2
	setCursor timeCol,cl
	printNum gameValue
	
	;Coloca acumulado
	mov cl,timeRow
	add cl,4
	setCursor timeCol,cl
	printNum selectedValue
	
	;Coloca a pontuacao
	mov cl,timeRow
	add cl,6
	setCursor timeCol,cl
	printWord gamePoints
	
	;Posiciona cursor para a primeira celula
	mov ah,startTabCol
	mov activeCol,ah
	mov ah,startTabRow
	mov activeRow,ah
	setCursor activeCol,activeRow
	
	call getTotalGame
	;Indice do vector
	xor si,si
	
	;Contador
	mov ah,2ch
	int 21h
	mov currentSec,dh ;Primeiro termo
	jmp doNextSecond
	
teclaNormal:
	mov ah,8 ;Read keyboard input
	int 21h
	mov keyPressed,al
	
	mov ah,0Ch ;Clear keyboard buffer
	int 21h

	cmp keyPressed,'a'
	je moveEsquerda
	cmp keyPressed,'A'
	je moveEsquerda
	
	cmp keyPressed,'d'
	je moveDireita
	cmp keyPressed,'D'
	je moveDireita
	
	cmp keyPressed,'s'
	je moveBaixo
	cmp keyPressed,'S'
	je moveBaixo
	
	cmp keyPressed,'w'
	je moveCima
	cmp keyPressed,'W'
	je moveCima
	
	cmp keyPressed,20h ;Space
	je doSelectNumber
	
	cmp keyPressed,1Bh ;ESC
	je doFinish
	
	cmp isGameOver,1
	je doShowSecond
	
	;Pausar o jogo
	cmp keyPressed,'p'
	je doPause
	cmp keyPressed,'P'
	je doPause
	
	cmp canSaveGame,0
	je doShowSecond
	
	cmp keyPressed,'g'
	je doSaveAndFinish
	cmp keyPressed,'G'
	je doSaveAndFinish
	
	jmp doShowSecond
	
doSelectNumber:
	call SelectNumber
	jmp doShowSecond

doPause:
	cmp canStopTimer,0
	je doShowSecond
	jmp doStopTimer

moveEsquerda:
	mov ah,startTabCol
	cmp activeCol,ah
	je doShowSecond
	sub activeCol,2
	dec si
	dec cellIdx
	jmp doShowSecond
	
moveDireita:
	mov ah,endTabCol
	cmp activeCol,ah
	je doShowSecond
	add activeCOl,2
	inc si
	inc cellIdx
	jmp doShowSecond
	
moveBaixo:
	mov ah,endTabRow
	cmp activeRow,ah
	je doShowSecond
	add activeRow,2
	xor ax,ax
	mov al,tabGameSize
	add si,ax
	add cellIdx,al
	jmp doShowSecond
	
moveCima:
	mov ah,startTabRow
	cmp activeRow,ah
	je doShowSecond
	sub activeRow,2
	xor ax,ax
	mov al,tabGameSize
	sub si,ax
	sub cellIdx,al
	jmp doShowSecond
	
doStopTimer:
	mov seconds,99d
	jmp doShowSecond
	
doSaveAndFinish:
	call tryAgain
	lea dx,fileGame
	call saveGame
	jmp doFinish
	
doNextSecond:
	setCursor activeCol,activeRow
	
	xor ax,ax
	mov al,gameValue
	cmp gameTotalValue,ax
	jb doEndGame
	
	mov ah,0Bh
	int 21h
	cmp al,0FFh ;Check keyboard buffer
	je teclaNormal
	
doShowSecond:
	cmp seconds,99d
	je doNextSecond

	mov ah,2Ch ;Obter segundos
	int 21h
	cmp currentSec,dh ;Segundos da hora
	je doNextSecond
	dec seconds
	
	setCursor timeCol,timeRow
	printNum seconds
	setCursor activeCol,activeRow
	mov currentSec,dh
	cmp seconds,0
	jne doNextSecond
	
	cmp seconds,0
	jne doWait	
doGameOver:
	mov isGameOver,1
	call refreshPontuacao
	setCursor timeCol,timeRow
	print msgGameOver	
	jmp doWait
doEndGame:
	setCursor timeCol,timeRow
	print msgWinner

doWait:
	call waitKeyboard
	
	cmp keyPressed,1Bh ;ESC - voltar ao menu
	jne doWait
doFinish:
	setCursor 0,0
	ret
startGame endp

selectNumber proc
	cmp tabGame[si],0
	je doFinish
	
	xor di,di
	xor ax,ax
	mov al,selectedCellCount
	add di,ax
	
	;Valor
	mov al,tabGame[si]
	add selectedValue,al
	mov selectedCells[di],al
	;Indice
	mov al,cellIdx
	mov selectedCellIndexes[di],al
	mov al,activeCol
	mov selectedCellCols[di],al
	mov al,activeRow
	mov selectedCellRows[di],al
	inc selectedCellCount
	;Remove o valor do tabuleiro
	mov tabGame[si],0
	printC ' '
	
	;Verificar se já atingiu o valor
	mov al,selectedValue
	cmp al,gameValue
	je doWin
	cmp al,gameValue
	ja doTryAgain
	jmp doRefreshValorAcumulado
	
doTryAgain:
	call tryAgain
	jmp doRefreshValorAcumulado
	
doWin:
	xor ax,ax
	mov al,selectedValue
	sub gameTotalValue,ax
	add gamePoints,10d
	mov selectedValue,0
	mov selectedCellCount,0
	mov al,maxSeconds
	mov seconds,al
	
	;Refresca a pontuacao
	mov cl,timeRow
	add cl,6
	setCursor timeCol,cl
	printWord gamePoints
	setCursor activeCol,activeRow

doRefreshValorAcumulado:
	;Coloca o valor acumulado
	mov cl,timeRow
	add cl,4
	setCursor timeCol,cl
	printNum selectedValue
	setCursor activeCol,activeRow
	jmp doFinish
	
doFinish:
	ret
selectNumber endp

tryAgain proc
	xor ax,ax
	xor bx,bx
	xor bp,bp
doTryAgainWhile:
	cmp bl,selectedCellCount
	je doTryAgainDone
	
	mov al,selectedCellIndexes[bx]
	mov bp,ax
	
	mov al,selectedCells[bx]
	mov tabGame[bp],al
	setCursor selectedCellCols[bx],selectedCellRows[bx]
	printD tabGame[bp]
	inc bx
	jmp doTryAgainWhile
doTryAgainDone:
	setCursor activeCol,activeRow
	mov selectedValue,0
	mov selectedCellCount,0
	ret
tryAgain endp

getTotalGame proc
	push si
	mov gameTotalValue,0
	;Indice do vector
	xor si,si
	;Contar total do tabuleiro
	xor ax,ax
	mov al,tabGameTotalSize
doGameCount:
	cmp si,ax
	je doGameCountFinish
	xor dx,dx
	mov dl,tabGame[si]
	add gameTotalValue,dx
	inc si
	jmp doGameCount
doGameCountFinish:
	pop si
	ret
getTotalGame endp

refreshPontuacao proc
	push cx
	push dx
	push si
	;Indice do vector
	xor si,si
	;Contar total do tabuleiro
	xor ax,ax
	xor cx,cx
	xor dx,dx
	mov al,tabGameTotalSize
doGameWhile:
	cmp si,ax
	je doGameCountFinish
	cmp tabGame[si],0
	je doNextNum
	;Pontos a retirar por cada número
	add dx,100d
doNextNum:
	inc si
	jmp doGameWhile
doGameCountFinish:
	sub dx,gamePoints	
	mov cl,timeRow
	add cl,6
	setCursor timeCol,cl
	printC '-'
	printWord dx
	setCursor activeCol,activeRow
	pop si
	pop dx
	pop cx
	ret
refreshPontuacao endp

showSavedGames proc
	call cleanScreen
	lea dx,screenLoadGame
	call readScreen
	;Verificar total de jogos
	mov loadGame,0
	lea dx,fileGame
	call readGame
	
	mov activeCol,35d
	mov activeRow,13d
	setCursor activeCol,activeRow
	mov cl,1	
	cmp totalGames,1 ;Jogo por defeito
	je doNoSavedGames
doWhile:
	cmp cl,totalGames
	je doKeyboard
	;Limite de jogos gravados
	cmp cl,maxGames
	je doNoSavedGames
	
	print msgJogoGravado
	printD cl
	inc cl
	mov activeCol,35d
	inc activeRow
	setCursor activeCol,activeRow
	jmp doWhile
	
doNoSavedGames:
	print msgNoSavedGames

doKeyboard:
	call waitKeyboard
	
	cmp keyPressed,1Bh ;ESC
	je doFinish
	
	cmp totalGames,1 ;Jogo por defeito
	je doKeyboard
	
	cmp keyPressed,'1'
	jb doKeyboard
	cmp keyPressed,'9'
	ja doKeyboard
	
	mov al,keyPressed
	sub al,30h
	cmp al,totalGames
	ja doKeyboard
	
	mov loadGame,al
doFinish:
    setCursor 0,0
	ret
showSavedGames endp

showConfig proc
	call cleanScreen
	lea dx,screenConfig
	call readScreen
	
	mov activeCol,25h
	mov activeRow,9h
	setCursor activeCol,activeRow
    ;Tempo
	printNum maxSeconds
	
    ;Valor a atingir
	inc activeRow
	setCursor activeCol,activeRow
	printNum gameValue
	
    ;Celula multi-color
	inc activeRow
	setCursor activeCol,activeRow
	printC '['
	cmp cellMultiColor,1
	jne flagMultiColorFalse
	printC 'X'
	jmp flagMultiColor
flagMultiColorFalse:
	printC ' '
flagMultiColor:
	printC ']'
	
    ;Gravar jogo
	inc activeRow
	setCursor activeCol,activeRow
	printC '['
	cmp canSaveGame,1
	jne flagSaveGameFalse
	printC 'X'
	jmp flagSaveGame
flagSaveGameFalse:
	printC ' '
flagSaveGame:
	printC ']'
	
    ;Pausar tempo
	inc activeRow
	setCursor activeCol,activeRow
	printC '['
	cmp canStopTimer,1
	jne flagStopTimerFalse
	printC 'X'
	jmp flagStopTimer
flagStopTimerFalse:
	printC ' '
flagStopTimer:
	printC ']'
	
	inc activeCol
	setCursor activeCol,activeRow
	;Numero de opcoes
	mov al,numOptions
	mov cellIdx,al
	
doKeyboard:
	call waitKeyboard
	
	cmp keyPressed,1Bh ;ESC
	je doFinish
	
	cmp keyPressed,4Dh ;Direita
	je doMoveRight

	cmp keyPressed,4Bh ;Esquerda
	je doMoveLeft

	cmp keyPressed,48h ;Cima
	je doMoveUp

	cmp keyPressed,50h ;Baixo
	je doMoveDown
	
	jmp doKeyboard
	
doMoveUp:
	cmp cellIdx,1
	je doKeyboard
	dec activeRow
	dec cellIdx
	setCursor activeCol,activeRow
	jmp doKeyboard
	
doMoveDown:
	mov al,numOptions
	cmp cellIdx,al
	je doKeyboard
	inc activeRow
	inc cellIdx
	setCursor activeCol,activeRow
	jmp doKeyboard
	
doMoveRight:
	cmp cellIdx,1
	je doIncOption1
	jmp doKeyboard
doIncOption1:
    ;Tempo
	inc maxSeconds
	dec activeCol
	setCursor activeCol,activeRow
	printNum maxSeconds
	inc activeCol
	setCursor activeCol,activeRow
	jmp doKeyboard

doMoveLeft:
	cmp cellIdx,1
	je doDecOption1
	jmp doKeyboard
doDecOption1:
    ;Tempo
	dec maxSeconds
	dec activeCol
	setCursor activeCol,activeRow
	printNum maxSeconds
	inc activeCol
	setCursor activeCol,activeRow
	jmp doKeyboard

doFinish:
    setCursor 0,0
	ret
showConfig endp