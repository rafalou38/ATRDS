set -xe
gcc -o atrds.out -std=gnu99 -Werror -Wall -Wextra -Wvla -fsanitize=address,undefined -O0 -g ./src/main.c

./atrds.out