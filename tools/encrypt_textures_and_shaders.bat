ECHO Encrypting Shaders...

set XOREncryptDecryptTool="%cd%/XOREncryptDecrypt.exe"

cd ..\src\platform\windows\data\shaders

%XOREncryptDecryptTool% shader_001_frag.cso shader_001_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_002_frag.cso shader_002_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_003_frag.cso shader_003_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_004_frag.cso shader_004_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_001_vert.cso shader_001_vert.ngs
ECHO .
%XOREncryptDecryptTool% shader_002_vert.cso shader_002_vert.ngs
ECHO .
%XOREncryptDecryptTool% shader_003_vert.cso shader_003_vert.ngs
ECHO .

del /s *.cso

ECHO Encrypting Textures...

cd ..\textures

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
