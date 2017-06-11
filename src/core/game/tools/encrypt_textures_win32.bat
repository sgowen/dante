ECHO Encrypting Textures...

cd ..\..\platform\windows\win32\data\textures

..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_001.dds texture_001.ngt
ECHO .
..\..\..\..\core\framework\tools\XOREncryptDecrypt.exe texture_002.dds texture_002.ngt
ECHO .

del /s *.dds
