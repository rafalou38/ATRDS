set -xe
gcc -o atrds.out -lm -std=c99 -Werror -Wall -Wextra -Wvla -fsanitize=address,undefined -O0 -g ./src/main.c ./src/grid.c ./src/shared.c ./src/enemies.c ./src/tower.c

./atrds.out "$@"