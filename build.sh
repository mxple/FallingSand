g++ -c -pg -g -O3 *.cpp && g++ *.o -o fallingSand -lsfml-graphics -lsfml-window -lsfml-system -pg && ./fallingSand 
rm *.o
