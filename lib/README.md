# How to get Dlls

I Used an in house script to copy all of the dlls from my computer into a single folder. if you would like this script

https://dock-it.dev/Bit-By-Byte/DLL-Extractor

it just takes a exe then copys all dlls that the exe needs into a folder in the working directory


```

PS C:\Users\User\OneDrive\Documents\Github\DLL-Extractor> & C:/Users/User/AppData/Local/Microsoft/WindowsApps/python3.11.exe c:/Users/User/OneDrive/Documents/Github/DLL-Extractor/main.py .\TesseractEngine.exe
Non-OS DLL Dependencies:
  libgcc_s_seh-1.dll
  glew32.dll
  glfw3.dll
  KERNEL32.dll
  msvcrt.dll
  OPENGL32.dll
  libwinpthread-1.dll
  SHELL32.dll
  libstdc++-6.dll
  USER32.dll
  libyaml-cpp.dll
  lua54.dll

Copied DLLs:
  libgcc_s_seh-1.dll
  glew32.dll
  glfw3.dll
  KERNEL32.dll
  msvcrt.dll
  OPENGL32.dll
  libwinpthread-1.dll
  SHELL32.dll
  libstdc++-6.dll
  USER32.dll
  libyaml-cpp.dll

```