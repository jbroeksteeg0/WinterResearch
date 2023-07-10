set -e

g++ src/*.cpp -o main -O3 -Wall -Wextra -std=c++20

time ./main $@
