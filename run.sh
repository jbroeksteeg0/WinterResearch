set -e

g++ src/*.cpp -o main -O3 -Wall -Wextra

time ./main $@
