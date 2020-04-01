# dante-assets

Project Dante features a completely data-driven system, relying on several .cfg files.

## Texture Work Flow

1. Do work inside the source folder (where .psd files are located)
2. Once a PSD sheet is complete, export a png to the "textures" folder
3. Open the "windows" folder and double-click on the "refresh_assets.bat"; this takes all of png files, converts them to dds files, and then encrypts them to (.ngt)
4. Open game_assets.cfg and define the new regions and/or animations you added
5. Do work in the Level Editor
6. Commit changes to BitBucket (you can use the same GitHub desktop tool from nosfuratu to do this)

## Shader Work Flow

1. Do work inside the shaders folder (where .hlsl files are located)
2. Once satisfied with shader changes, open the "windows" folder and double-click on the "refresh_assets.bat"; this compiles all of the hlsl files to cso files and then encrypts them to (.ngt)
3. If compilation succeeded, you will be able to run the game and see the results of your shader changes.
4. Commit changes to BitBucket (you can use the same GitHub desktop tool from nosfuratu to do this)

## Before Running the Game, do this EVERY time

1. Double-click the refresh_assets.bat file inside the windows folder
2. This is necessary to ensure that the latest textures and shaders are converted/compiled and encrypted

## CFG Files Breakdown

### global.cfg

1. This is where properties such as gravity and lighting are defined
2. Ideally, nothing in the game is hardcoded, and everything is configurable in this file.

### game_assets.cfg

This contains all of the texture mappings.

### entities.cfg

This is where the Studio reads/writes all of the entity definitions.

### maps.cfg

1. This is where maps are defined
2. Entries must each have a unique 4 character key (this is how the network tells the clients which map to load)
3. Each entry points to a file inside the config file, reprenting the file to load for a given key (e.g. "Z001": "zone_1.cfg")
