ECHO Encrypting Textures...

set XOREncryptDecryptTool="%cd%/XOREncryptDecrypt.exe"

cd ..\src\platform\windows\data\textures

%XOREncryptDecryptTool% texture_001.dds texture_001.ngt
ECHO .
%XOREncryptDecryptTool% texture_002.dds texture_002.ngt
ECHO .
%XOREncryptDecryptTool% texture_003.dds texture_003.ngt
ECHO .
%XOREncryptDecryptTool% texture_004.dds texture_004.ngt
ECHO .
%XOREncryptDecryptTool% texture_005.dds texture_005.ngt
ECHO .

del /s *.dds
