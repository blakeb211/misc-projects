#!/bin/bash
echo "Compiling client.cpp and server.cpp to build executables..."
clang++ -o client -Wall -Wextra -pedantic -g -std=c++17 client.cpp 
clang++ -o server -Wall -Wextra -pedantic -g -std=c++17 server.cpp

echo "****************************************************"
echo "To see the client and server connect,"
echo "run ./server in one terminal and ./client in another."
echo "****************************************************"
