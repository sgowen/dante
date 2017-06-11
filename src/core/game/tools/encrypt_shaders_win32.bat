ECHO Encrypting Shaders...

cd ..\..\platform\windows\win32\data\shaders

..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_001_frag.cso shader_001_frag.ngs
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_002_frag.cso shader_002_frag.ngs
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_003_frag.cso shader_003_frag.ngs
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_001_vert.cso shader_001_vert.ngs
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_002_vert.cso shader_002_vert.ngs
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_003_vert.cso shader_003_vert.ngs
ECHO .

del /s *.cso
