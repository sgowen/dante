#!/bin/bash

XOREncryptDecryptTool="$1/tools/$2"

cd "$1/src/core/framework/graphics/opengl/shader"

$XOREncryptDecryptTool shader_001_frag.fsh shader_001_frag.ngs
$XOREncryptDecryptTool shader_002_frag.fsh shader_002_frag.ngs
$XOREncryptDecryptTool shader_003_frag.fsh shader_003_frag.ngs
$XOREncryptDecryptTool shader_001_vert.vsh shader_001_vert.ngs
$XOREncryptDecryptTool shader_002_vert.vsh shader_002_vert.ngs
$XOREncryptDecryptTool shader_003_vert.vsh shader_003_vert.ngs

cd "$1/assets/textures"

$XOREncryptDecryptTool texture_001.png texture_001.ngt
$XOREncryptDecryptTool texture_002.png texture_002.ngt
$XOREncryptDecryptTool texture_003.png texture_003.ngt
$XOREncryptDecryptTool texture_004.png texture_004.ngt
$XOREncryptDecryptTool texture_005.png texture_005.ngt
