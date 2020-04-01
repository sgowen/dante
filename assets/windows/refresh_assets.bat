set FXC="%cd%\fxc.exe"
set TEXCONV="%cd%\texconv.exe"
set XOREncryptDecryptTool="%cd%\XOREncryptDecrypt.exe"

ECHO Compiling Shaders...
cd ..\shaders\
%FXC% /T vs_4_0 /Fo shader_001_vert.cso shader_001_vert.hlsl
%FXC% /T vs_4_0 /Fo shader_002_vert.cso shader_002_vert.hlsl
%FXC% /T vs_4_0 /Fo shader_003_vert.cso shader_003_vert.hlsl
%FXC% /T vs_4_0 /Fo shader_004_vert.cso shader_004_vert.hlsl
%FXC% /T ps_4_0 /Fo shader_001_frag.cso shader_001_frag.hlsl
%FXC% /T ps_4_0 /Fo shader_002_frag.cso shader_002_frag.hlsl
%FXC% /T ps_4_0 /Fo shader_003_frag.cso shader_003_frag.hlsl
%FXC% /T ps_4_0 /Fo shader_004_frag.cso shader_004_frag.hlsl
%FXC% /T ps_4_0 /Fo shader_005_frag.cso shader_005_frag.hlsl

ECHO Encrypting Shaders...
%XOREncryptDecryptTool% shader_001_vert.cso shader_001_vert.ngs
%XOREncryptDecryptTool% shader_002_vert.cso shader_002_vert.ngs
%XOREncryptDecryptTool% shader_003_vert.cso shader_003_vert.ngs
%XOREncryptDecryptTool% shader_004_vert.cso shader_004_vert.ngs
%XOREncryptDecryptTool% shader_001_frag.cso shader_001_frag.ngs
%XOREncryptDecryptTool% shader_002_frag.cso shader_002_frag.ngs
%XOREncryptDecryptTool% shader_003_frag.cso shader_003_frag.ngs
%XOREncryptDecryptTool% shader_004_frag.cso shader_004_frag.ngs
%XOREncryptDecryptTool% shader_005_frag.cso shader_005_frag.ngs
del /s *.cso

ECHO Converting PNG Textures to DDS Textures...
cd ..\textures\
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_000.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_001.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_002.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_003.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_004.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_005.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_006.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_007.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_008.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_009.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM texture_010.png

ECHO Converting PNG Normal Maps to DDS Textures...
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_001.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_002.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_003.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_004.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_005.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_006.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_007.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_008.png
%TEXCONV% -m 4 -nologo -srgb -f B8G8R8A8_UNORM n_texture_009.png

ECHO Encrypting Textures...
%XOREncryptDecryptTool% texture_000.dds texture_000.ngt
%XOREncryptDecryptTool% texture_001.dds texture_001.ngt
%XOREncryptDecryptTool% texture_002.dds texture_002.ngt
%XOREncryptDecryptTool% texture_003.dds texture_003.ngt
%XOREncryptDecryptTool% texture_004.dds texture_004.ngt
%XOREncryptDecryptTool% texture_005.dds texture_005.ngt
%XOREncryptDecryptTool% texture_006.dds texture_006.ngt
%XOREncryptDecryptTool% texture_007.dds texture_007.ngt
%XOREncryptDecryptTool% texture_008.dds texture_008.ngt
%XOREncryptDecryptTool% texture_009.dds texture_009.ngt
%XOREncryptDecryptTool% texture_010.dds texture_010.ngt

ECHO Encrypting Normal Maps...
%XOREncryptDecryptTool% n_texture_001.dds n_texture_001.ngt
%XOREncryptDecryptTool% n_texture_002.dds n_texture_002.ngt
%XOREncryptDecryptTool% n_texture_003.dds n_texture_003.ngt
%XOREncryptDecryptTool% n_texture_004.dds n_texture_004.ngt
%XOREncryptDecryptTool% n_texture_005.dds n_texture_005.ngt
%XOREncryptDecryptTool% n_texture_006.dds n_texture_006.ngt
%XOREncryptDecryptTool% n_texture_007.dds n_texture_007.ngt
%XOREncryptDecryptTool% n_texture_008.dds n_texture_008.ngt
%XOREncryptDecryptTool% n_texture_009.dds n_texture_009.ngt
del /s *.dds

@pause
