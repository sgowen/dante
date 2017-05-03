ECHO Encrypting Shaders...

cd ..\..\..\platform\windows\win32\data\shaders

..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_001_frag.cso shader_001_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_002_frag.cso shader_002_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_003_frag.cso shader_003_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_001_vert.cso shader_001_vert.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_002_vert.cso shader_002_vert.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_003_vert.cso shader_003_vert.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_004_frag.cso shader_004_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_005_frag.cso shader_005_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_006_frag.cso shader_006_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_007_frag.cso shader_007_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_008_frag.cso shader_008_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_009_frag.cso shader_009_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_010_frag.cso shader_010_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_011_frag.cso shader_011_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_012_frag.cso shader_012_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_013_frag.cso shader_013_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_014_frag.cso shader_014_frag.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_008_vert.cso shader_008_vert.ngs
ECHO .
..\..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe shader_010_vert.cso shader_010_vert.ngs
ECHO .

del /s *.cso