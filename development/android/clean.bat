@echo off

rmdir /S /Q "assets"
rmdir /S /Q "bin"
rmdir /S /Q "gen"
rmdir /S /Q "libs"
rmdir /S /Q "obj"

rename "C:\Development\c++\android\SDL2" "SDL2_"
rmdir /S /Q "jni\SDL2"
rename "C:\Development\c++\android\SDL2_" "SDL2"

rename "C:\Development\c++\android\SDL2_image" "SDL2_image_"
rmdir /S /Q "jni\SDL2_image"
rename "C:\Development\c++\android\SDL2_image_" "SDL2_image"

rename "C:\Development\c++\android\SDL2_mixer" "SDL2_mixer_"
rmdir /S /Q "jni\SDL2_mixer"
rename "C:\Development\c++\android\SDL2_mixer_" "SDL2_mixer"

rename "C:\Development\c++\android\raknet\raknet" "raknet_"
rmdir /S /Q "jni\RakNet"
rename "C:\Development\c++\android\raknet\raknet_" "raknet"

rename "C:\Development\c++\boost" "boost_"
rmdir /S /Q "jni\boost"
rename "C:\Development\c++\boost_" "boost"

del "jni\src\Android.mk"

del "ant.properties"
del "local.properties"
