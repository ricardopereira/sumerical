;Tecnologias e Arquitecturas de Computadores
;Trabalho Pratico - ISEC 2013
;Ricardo Pereira - 21220335
;Goncalo Serrano - 21220334
.8086
.model small
.stack 2048

dseg segment para public 'data'
	activeCol byte 0
	activeRow byte 0
	keyPressed byte 0
	par byte 0
	randomNum byte 0
	numInStr db "  $"
	string byte 6 dup(?)
	Count word 0
	
	;Timer
	seconds byte 0
	currentSec byte 0
	timeCol byte 58d
	timeRow byte 10d
	
	;Video
	videoCol byte 10d
	videoRow byte 3d
	videoIdx byte 0
	videoLastCell byte 0
	
	;Constantes
	tabGameSizeDefault byte 10d
	maxSecondsDefault byte 20d
	maxGameValueDefault byte 36d ;(quatro numeros de 9)
	
	;Extras/Configuracoes
	cellMultiColor byte 0
	canSaveGame byte 1
	canStopTimer byte 1
	maxSeconds byte 20d
	maxGameValue byte 36d
	tabGameSize byte 10d ;Tamanho do Tabuleiro
	
	numOptions byte 6
	
	;Tabuleiro
	tabGame byte 100d dup(0) ;Números de cada célula
	tabGameLast word 0 ;Última célula
	tabGameTotalSize byte 0 ;Número total de celulas
	cellIdx byte 0
	isGameOver byte 0
	gameTotalValue word 0 ;Total dos números das células
	gamePoints word 0 ;Pontuacao
	gameValue byte 0 ;Valor a atingir em jogo
	selectedCells byte 100d dup(0)
	selectedCellCount byte 0
	selectedCellIndexes byte 100d dup(0)
	selectedCellRows byte 100d dup(0)
	selectedCellCols byte 100d dup(0)
	selectedValue byte 0
	;Limites do Tabuleiro
	startTabCol byte 0
	startTabRow byte 0
	endTabCol byte 0
	endTabRow byte 0
	
	;Ficheiro de Jogo
	fileHandle word 0
	fileBf byte 0
	fileGameIdx byte 0
	fileLine byte 0
	fileGame byte 'games.txt',0
	
	;Carregamento do Jogo
	totalGames byte 0
	maxGames byte 9
	loadGame byte 0 ;ID de jogo
	lineGameID byte 0
	lineGameTabSize byte 0
	lineGameTab byte 0
	
	;Interface
	screenWelcome byte 'welcome.int',0
	screenMenu byte 'menu.int',0
	screenGame byte 'game.int',0
	screenLoadGame byte 'loadgame.int',0
	screenConfig byte 'config.int',0
	screenReinit byte 'reinit.int',0
	
	;Mensagens
	msgJogoGravado db "Gravado $"
	msgNoSavedGames db "Sem jogos$"
	msgGameOver db "GAME OVER$"
	msgWinner db "FIM$"
	;Mensagens de Erro
	msgErrorCreate db "Ocorreu um erro na criacao do ficheiro$"
	msgErrorOpen db "Ocorreu um erro na abertura do ficheiro$"
	msgErrorRead db "Ocorreu um erro na leitura para ficheiro$"
	msgErrorWrite db "Ocorreu um erro na escrita para ficheiro$"
	msgErrorClose db "Ocorreu um erro no fecho do ficheiro$"
	msgErrorFileFormat db "Ficheiro invalido$"
dseg ends

cseg segment para public 'code'
	 assume  cs:cseg, ds:dseg
	
;Includes
include tools.inc
include screen.inc
include db.inc
include game.inc
	
main proc
    mov ax,0B800H
    mov es,ax
	
	mov ax,dseg
	mov ds,ax
	
	;Modo de Video: 25x80
	mov ah,0h
	mov al,3h ; 80x25 16 color text (cga,ega,mcga,vga)
	int 10h
	
	call cleanScreen
	setCursor activeCol,activeRow
	
	;Carregar interface - Welcome
	lea dx,screenWelcome
	call readScreen
	;Prima qualquer tecla
	mov ah,01h
	int 21h
	
	call menu

	mov ah,4CH
	int 21h
main endp

menu proc
doInit:
	;Carregar interface - Menu
	call cleanScreen
	lea dx,screenMenu
	call readScreen
doMenu:		
	call waitKeyboard

	cmp keyPressed,'1' ;Iniciar jogo
	je doNovoJogo
	
	cmp keyPressed,'2' ;Reiniciar jogo
	je doReiniciarJogo
	
	cmp keyPressed,'3' ;Preparar jogo
	je doPrepararJogo
	
	cmp keyPressed,'4' ;Carregar jogo
	je doCarregarJogo
	
	cmp keyPressed,'5' ;Carregar jogo
	je doCarregarJogoDinamico

	cmp keyPressed,'0' ;SAIR
	je doFinish
	cmp keyPressed,1Bh ;ESC
	je doFinish

	jmp doMenu
	
doNovoJogo:
	mov loadGame,0
doJogo:
	call cleanScreen
	lea dx,screenGame
	call readScreen
	;Carregar jogo
	lea dx,fileGame
	call readGame
	call startGame
	jmp doInit
	
doReiniciarJogo:
	call loadOriginalGame
	jmp doInit
	
doPrepararJogo:
	call showConfig
	jmp doInit
	
doCarregarJogo:
	call showSavedGames
	cmp loadGame,0 ;Nao seleccinou nenhum jogo
	je doInit
	jmp doJogo
	
doCarregarJogoDinamico:
	call cleanScreen
	lea dx,screenGame
	call readScreen
	;Carregar jogo
	call randomGame
	call startGame
	jmp doInit

doFinish:
	ret
menu endp

cseg ends
end main