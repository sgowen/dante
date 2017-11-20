ECHO Encrypting Shaders...

set XOREncryptDecryptTool=$(pwd)/XOREncryptDecrypt.exe

cd ..\src\platform\windows\data\shaders

%XOREncryptDecryptTool% shader_001_frag.cso shader_001_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_002_frag.cso shader_002_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_003_frag.cso shader_003_frag.ngs
ECHO .
%XOREncryptDecryptTool% shader_001_vert.cso shader_001_vert.ngs
ECHO .
%XOREncryptDecryptTool% shader_002_vert.cso shader_002_vert.ngs
ECHO .
%XOREncryptDecryptTool% shader_003_vert.cso shader_003_vert.ngs
ECHO .

del /s *.cso
