@echo off

REM
REM �����[�X�o�C�i���̃R�s�[
REM
copy /y release\*.exe .
copy /y release\*.dll .

REM
REM release/debug�t�H���_�Esuo/aps�����폜
REM
rd  /s /q release debug
del /a:h *.suo
del /s *.aps *.ncb *.vcproj.*.user

REM
REM �T�u�t�H���_�ȉ���release/debug�t�H���_���폜
REM
for /d %%f in (tc2ch\*) do rd /s /q %%f\debug %%f\release

REM pause
echo .

