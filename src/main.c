#include <stdio.h>

int main(){
    printf("Hellow world\n");

    int w = 40;
    int h = 20;
    for(int x = 0; x<w; x++){
        for(int y = 0; y<h; y++){
            if(y%5 <= 3 && x%3 > 0){
                printf("█");        
            }
            else{
                printf(" ");
            }
        }   
        printf("\n");
    }
    return 0;
}
