CC=g++

CC_GLFW_INCLUDE=C:\SDK\glfw\include
CC_GLFW_LIB=C:\SDK\glfw\x64\lib-mingw-w64
CC_STB_INCLUDE=C:\SDK\stb
CC_DGE_INCLUDE=..\..\..\..\defGameEngine

CC_LIBS=-lgdi32 -luser32 -lkernel32 -lopengl32 -lglfw3dll -lglu32

CC_FLAGS=-Wall -pedantic -std=c++20 -fms-extensions
CC_DEPENDENCIES=-I"$(CC_STB_INCLUDE)" -I"$(CC_GLFW_INCLUDE)" -I"$(CC_DGE_INCLUDE)" -L"$(CC_GLFW_LIB)" $(CC_LIBS)

.PHONY: all
all:
	$(CC) $(CC_FLAGS) -o ..\..\Target\Game.exe ..\..\..\Sources\Main.cpp $(CC_DEPENDENCIES)
