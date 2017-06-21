ECHO Encrypting Textures...

cd ..\..\..\platform\windows\data\textures

..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_001.dds texture_001.ngt
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_002.dds texture_002.ngt
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_003.dds texture_003.ngt
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_004.dds texture_004.ngt
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_005.dds texture_005.ngt
ECHO .

del /s *.dds
