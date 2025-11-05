all: compile link

compile:
	g++ -c main.cpp -I"C:\Users\Giang\Documents\libraries\SFML-3.0.2\include" -DSFML_STATIC

link:
	g++ main.o -o main -L"C:\Users\Giang\Documents\libraries\SFML-3.0.2\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows -lsfml-main

clean:
	del /f *.exe *.o