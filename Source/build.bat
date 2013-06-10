@echo off
CD "Source"
ml main.asm
IF %ERRORLEVEL% EQU 0 GOTO RUN
ECHO ERRO
PAUSE

GOTO EXIT
:RUN
  main.exe

:EXIT