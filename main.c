#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#define BOUND_CHECK(pos,size) (pos >= 0 && pos < size)
#define IS_ALPHA(arr,size,pos) ((arr[pos] >= 'a' && arr[pos] <= 'z') && BOUND_CHECK(pos,size))
#define IS_VOWEL(c) ((c == 'a')||(c == 'i')||(c == 'u')||(c == 'e')||(c == 'o'))
#define SET 2

typedef struct {
    uint16_t begin,end;
    uint8_t flag;
}wSplt_t;

typedef struct {
    uint8_t begin,flag,vow,sCons,mCons,prevChar,currChar;
}sSplt_t;

const uint16_t WLEN = 4096;
wSplt_t wReg;
sSplt_t sReg;

uint16_t parseStr(char *buff){
    printf("Masukan kalimat (maks. %d karakter):\n", WLEN-1);
    fgets(buff, WLEN, stdin);
    uint16_t len = strlen(buff);
    for(uint16_t i = 0;i < len;i++){
        buff[i] = tolower(buff[i]);
    }
    return len;
}

void parseWord(char *buff, uint16_t len, uint16_t i){
    if(!IS_ALPHA(buff,len,i)){
        printf("%c", buff[i]);
    }else{
        if(!IS_ALPHA(buff,len,i-1) && IS_ALPHA(buff,len,i)) wReg.begin=i,wReg.flag++;
        if(!IS_ALPHA(buff,len,i+1) && IS_ALPHA(buff,len,i)) wReg.end=i,wReg.flag++;
    }
}

void rstCount(){
    sReg.sCons=0;sReg.mCons=0;sReg.vow=0;sReg.prevChar = ' ';
}

void printSyll(char *buff, uint16_t n, char *add){
    for(uint16_t i = sReg.begin; i <= n;i++)printf("%c", buff[i]);
    printf("%s", add);
}

void parseSyll(char *buff, uint16_t *n){
    switch(sReg.sCons){
        case 0 : *n -= 1;break;
        case 1 : *n -= (1 + sReg.sCons + sReg.mCons);break;
        default: *n -= (1 + (sReg.sCons - 1 - sReg.mCons)+(2 * sReg.mCons));break;
    }
    printSyll(buff, *n, "-");
    sReg.begin=*n+1;rstCount();
}

void vowelCheck(){
    switch(sReg.prevChar){
        case 'a': if(sReg.currChar != 'i'&& sReg.currChar != 'u') sReg.vow++;break;
        case 'e': if(sReg.currChar != 'i'&& sReg.currChar != 'u') sReg.vow++;break;
        case 'o': if(sReg.currChar != 'i') sReg.vow++;break;
        default : sReg.vow++;
    }
}

void consonantCheck(){
    if(sReg.sCons == 1 && sReg.mCons > 0) sReg.mCons--;
    switch(sReg.prevChar){
        case 'n':(sReg.currChar != 'y'&& sReg.currChar != 'g') ?
                 sReg.sCons++ : sReg.mCons++;break;
        case 's':(sReg.currChar != 'y') ? sReg.sCons++ : sReg.mCons++;break;
        case 'k':(sReg.currChar != 'h') ? sReg.sCons++ : sReg.mCons++;break;
        default:sReg.sCons++;
    }
}

void strideWord(char *buff){
    wReg.flag = 0;
    sReg.begin=wReg.begin;rstCount();
    for(uint16_t i = wReg.begin;i <= wReg.end;i++){
        sReg.currChar = buff[i];
        if(IS_VOWEL(sReg.currChar)){
            vowelCheck();
            if(sReg.vow == SET) parseSyll(buff, &i);
        }else if(sReg.vow)
            consonantCheck();
        sReg.prevChar = sReg.currChar;
    }
    printSyll(buff, wReg.end,"");
}

void syllSplit(char *buff, uint16_t len){
    wReg.flag=0;
    for(uint16_t i = 0;i < len;i++){
        parseWord(buff, len, i);
        if(wReg.flag == SET) strideWord(buff);
    }
}

int main() {
    char line[WLEN];
    uint16_t len = parseStr(line);
    syllSplit(line, len);
    return 0;
}