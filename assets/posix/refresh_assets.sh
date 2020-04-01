#!/bin/bash

XOREncryptDecryptTool="$1/assets/posix/$2"

echo "Encrypting Shaders..."
cd "$1/assets/shaders"
$XOREncryptDecryptTool shader_001_vert.vsh shader_001_vert.ngs
$XOREncryptDecryptTool shader_002_vert.vsh shader_002_vert.ngs
$XOREncryptDecryptTool shader_003_vert.vsh shader_003_vert.ngs
$XOREncryptDecryptTool shader_004_vert.vsh shader_004_vert.ngs
$XOREncryptDecryptTool shader_001_frag.fsh shader_001_frag.ngs
$XOREncryptDecryptTool shader_002_frag.fsh shader_002_frag.ngs
$XOREncryptDecryptTool shader_003_frag.fsh shader_003_frag.ngs
$XOREncryptDecryptTool shader_004_frag.fsh shader_004_frag.ngs
$XOREncryptDecryptTool shader_005_frag.fsh shader_005_frag.ngs

echo "Encrypting Textures..."
cd "$1/assets/textures"
$XOREncryptDecryptTool texture_000.png texture_000.ngt
$XOREncryptDecryptTool texture_001.png texture_001.ngt
$XOREncryptDecryptTool texture_002.png texture_002.ngt
$XOREncryptDecryptTool texture_003.png texture_003.ngt
$XOREncryptDecryptTool texture_004.png texture_004.ngt
$XOREncryptDecryptTool texture_005.png texture_005.ngt
$XOREncryptDecryptTool texture_006.png texture_006.ngt
$XOREncryptDecryptTool texture_007.png texture_007.ngt
$XOREncryptDecryptTool texture_008.png texture_008.ngt
$XOREncryptDecryptTool texture_009.png texture_009.ngt
$XOREncryptDecryptTool texture_010.png texture_010.ngt

echo "Encrypting Normal Maps..."
$XOREncryptDecryptTool n_texture_001.png n_texture_001.ngt
$XOREncryptDecryptTool n_texture_002.png n_texture_002.ngt
$XOREncryptDecryptTool n_texture_003.png n_texture_003.ngt
$XOREncryptDecryptTool n_texture_004.png n_texture_004.ngt
$XOREncryptDecryptTool n_texture_005.png n_texture_005.ngt
$XOREncryptDecryptTool n_texture_006.png n_texture_006.ngt
$XOREncryptDecryptTool n_texture_007.png n_texture_007.ngt
$XOREncryptDecryptTool n_texture_008.png n_texture_008.ngt
$XOREncryptDecryptTool n_texture_009.png n_texture_009.ngt
