#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BOUND_CHECK(pos,size) (pos >= 0 && pos < size)
#define IS_ALPHA(arr,size,pos) ((arr[pos] >= 'a' && arr[pos] <= 'z') && BOUND_CHECK(pos,size))
#define IS_VOWEL(c) ((c == 'a')||(c == 'i')||(c == 'u')||(c == 'e')||(c == 'o'))

const int WLEN = 1024;

int parseStr(char *buff){
    printf("Masukan kalimat (maks. %d karakter):\n", WLEN-1);
    fgets(buff, WLEN, stdin);
    int len = strlen(buff);
    for(int i = 0;i < len;i++){
        buff[i] = tolower(buff[i]);
    }
    return len;
}

void syllSplt(char *buff, int len){
    int wb,we,wf=0;
    for(int i = 0;i < len;i++){
        if(!IS_ALPHA(buff,len,i)){
            printf("%c", buff[i]);
        }else{
            if(!IS_ALPHA(buff,len,i-1) && IS_ALPHA(buff,len,i)) wb=i,wf++;
            if(!IS_ALPHA(buff,len,i+1) && IS_ALPHA(buff,len,i)) we=i,wf++;
        }
        if(wf == 2){
            wf = 0;
            int sb=wb,cons=0,mcons=0,vow=0;
            char prevChar = ' ';
            for(int j = wb;j <= we;j++){
                char currChar = buff[j];
                if(IS_VOWEL(currChar)){
                    switch (prevChar) {
                        case 'a':if(currChar != 'i'&&currChar != 'u') vow++;break;
                        case 'e':if(currChar != 'i'&&currChar != 'u') vow++;break;
                        case 'o':if(currChar != 'i') vow++;break;
                        default:vow++;
                    }
                    if(vow == 2){
                        switch(cons){
                            case 0: j -= 1;break;
                            case 1: j -= (1+cons+mcons);break;
                            default: j -= (1+(cons-1-mcons)+(2*mcons));break;
                        }
                        for(int k = sb; k <= j;k++)printf("%c", buff[k]);
                        printf("-");
                        vow = 0,cons=0,mcons=0,sb = j+1;
                    }
                }else if(vow){
                    if(cons == 1 && mcons > 0) mcons--;
                    switch (prevChar) {
                        case 'n':(currChar != 'y'&&currChar != 'g') ? cons++ : mcons++;break;
                        case 's':(currChar != 'y') ? cons++ : mcons++;break;
                        case 'k':(currChar != 'h') ? cons++ : mcons++;break;
                        default:cons++;
                    }
                }
                prevChar = currChar;
            }
            for(int j = sb; j <= we;j++)printf("%c", buff[j]);
        }
    }
}

int main() {
    char line[WLEN];
    int len = parseStr(line);
    syllSplt(line, len);
    return 0;
}