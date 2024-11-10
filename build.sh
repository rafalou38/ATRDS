set -xe

CMD="gcc -o atrds.out -std=c99"
CMD="$CMD -Werror -Wall -Wformat -Wextra -Wvla"
CMD="$CMD -fsanitize=address,undefined -O0 -g"
CMD="$CMD ./src/main.c ./src/grid.c ./src/shared.c ./src/enemies.c ./src/tower.c ./src/ncursesWrapper.c"
CMD="$CMD -lm -lncurses"
eval $CMD
