#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

// Periksa Batasan Array.
#define BOUND_CHECK(pos,size) (pos >= 0 && pos < size)
// Periksa Karakter Alfabet.
#define IS_ALPHA(arr,size,pos) ((arr[pos] >= 'a' && arr[pos] <= 'z') && BOUND_CHECK(pos,size))
// Periksa Huruf Vokal.
#define IS_VOWEL(c) ((c == 'a')||(c == 'i')||(c == 'u')||(c == 'e')||(c == 'o'))
// Flag Set untuk Register.
#define SET 2
// Maksimum Karakter pada Teks.
#define WLEN 4096

// Definisi Tipe Data Register Word Splitter.
// Member:
// - begin <uint16_t>: Posisi karakter awal kata.
// - end   <uint16_t>: Posisi karakter akhir kata.
// - flag  <uint8_t> : Status operasi pemecah kata.
typedef struct {
    uint16_t begin,end;
    uint8_t flag;
}wSplt_t;

// Definisi Tipe Data Register Syllable Splitter.
// Member:
// - begin    <uint16_t>: Posisi karakter awal suku kata.
// - vow      <uint8_t> : Jumlah huruf vokal (sebagai flag).
// - sCons    <uint8_t> : Jumlah huruf konsonan tunggal & jamak.
// - mCons    <uint8_t> : Jumlah huruf konsonan jamak.
// - prevChar <uint8_t> : Karakter yang sebelumnya dibaca.
// - currChar <uint8_t> : Karakter yang dibaca sekarang.
typedef struct {
    uint16_t begin;
    uint8_t vow,sCons,mCons,prevChar,currChar;
}sSplt_t;

// Variabel Kerja Global
wSplt_t wReg;
sSplt_t sReg;

// Parsing Data stdin ke Buffer. Ubah Semua Karakter ke Lowercase.
// Parameter:
// - buff <char*>: Array karakter untuk menyimpan data.
// Return Value:
// - len <uint16_t>: Panjang string (termasuk NULL terminator).
uint16_t parseStr(char *buff){
    printf("Masukan kalimat (maks. %d karakter):\n", WLEN-1);
    fgets(buff, WLEN, stdin);
    uint16_t len = strlen(buff);
    for(uint16_t i = 0;i < len;i++)
        buff[i] = tolower(buff[i]); // Ubah ke lowercase.
    return len;
}

// Parsing Kata dari Teks yang Diberikan. Simpan Data ke Register.
// Parameter:
// - buff <char*>   : Buffer penyimpanan teks.
// - len  <uint16_t>: Panjang buffer (untuk bounding check).
// - i    <uint16_t>: Indeks pembacaan karakter pada teks.
void parseWord(char *buff, uint16_t len, uint16_t i){
    if(!IS_ALPHA(buff,len,i)){
        printf("%c", buff[i]);  // Cetak langsung karakter non alfabet.
    }else{
        // Periksa awal kata.
        if(!IS_ALPHA(buff,len,i-1) && IS_ALPHA(buff,len,i)) wReg.begin=i,wReg.flag++;
        // Periksa akhir kata.
        if(!IS_ALPHA(buff,len,i+1) && IS_ALPHA(buff,len,i)) wReg.end=i,wReg.flag++;
    }
}

// Kembalikan Status Pencacah Huruf ke Nol. Simpan Perubahan ke Register.
void rstCount(){
    sReg.sCons=0;sReg.mCons=0;sReg.vow=0;sReg.prevChar = ' ';
}

// Cetak Setiap Karakter pada Suku Kata. Tambahkan Separator untuk Tengah Kata.
// Parameter:
// - buff <char*>   : Buffer penyimpanan teks.
// - n    <uint16_t>: Indeks karakter terakhir pada suku kata.
// - add  <char*>   : Separator suku kata (opsional).
void printSyll(char *buff, uint16_t n, char *add){
    for(uint16_t i = sReg.begin; i <= n;i++)printf("%c", buff[i]);
    printf("%s", add);
}

// Parsing Suku Kata pada Kata. Operasi Register.
// Parameter:
// - buff <char*>    : Buffer penyimpanan teks.
// - n    <uint16_t*>: Indeks pembacaan karakter pada kata.
void parseSyll(char *buff, uint16_t *n){
    switch(sReg.sCons){
        case 0 : *n -= 1;break;
        case 1 : *n -= (1 + sReg.sCons + sReg.mCons);break;
        default: *n -= (1 + (sReg.sCons - 1 - sReg.mCons)+(2 * sReg.mCons));break;
    }   // Ubah indeks mundur ke akhir suku kata.
    printSyll(buff, *n, "-"); // Cetak suku kata beserta separator.
    sReg.begin=*n+1;rstCount();    // Karakter awal tepat setelah karakter akhir.
}

// Periksa Huruf Vokal beserta Diftong/Monoftong. Simpan Pencacahan ke Register.
void vowelCheck(){
    switch(sReg.prevChar){
        case 'a': if(sReg.currChar != 'i'&& sReg.currChar != 'u') sReg.vow++;break;
        case 'e': if(sReg.currChar != 'i'&& sReg.currChar != 'u') sReg.vow++;break;
        case 'o': if(sReg.currChar != 'i') sReg.vow++;break;
        default : sReg.vow++;
    }   // Jika diftong/monoftong ditemukan, cukup cacah sekali.
}

// Periksa Huruf Konsonan beserta konsonan jamak. Simpan Pencacahan ke Register.
void consonantCheck(){
    if(sReg.sCons == 1 && sReg.mCons > 0) sReg.mCons--; // Hapus konsonan jamak sebelum pemisah.
    switch(sReg.prevChar){
        case 'n':(sReg.currChar != 'y'&& sReg.currChar != 'g') ?
                 sReg.sCons++ : sReg.mCons++;break;
        case 's':(sReg.currChar != 'y') ? sReg.sCons++ : sReg.mCons++;break;
        case 'k':(sReg.currChar != 'h') ? sReg.sCons++ : sReg.mCons++;break;
        default:sReg.sCons++; // Konsonan jamak masuk ke konsonan tunggal untuk klasifikasi.
    }   // Data terpisah konsonan jamak untuk kalkulasi lokasi pemisah suku kata.
}

// Periksa Setiap Karakter pada Kata secara berurutan. Operasi Penuh Pemisah Suku Kata.
// Parameter:
// - buff <char*>: Buffer penyimpanan teks.
void strideWord(char *buff){
    wReg.flag = 0;  // Reset flag word splitter.
    sReg.begin=wReg.begin;rstCount();  // Inisialisasi register syllable splitter.
    for(uint16_t i = wReg.begin;i <= wReg.end;i++){ // Untuk setiap huruf pada kata.
        sReg.currChar = buff[i];
        if(IS_VOWEL(sReg.currChar)){   // Huruf vokal sebagai flag syllable splitter.
            vowelCheck();
            if(sReg.vow == SET) parseSyll(buff, &i); // Cetak suku kata yang ditemukan.
        }else if(sReg.vow)             // Hanya baca konsonan setelah huruf vokal pertama.
            consonantCheck();
        sReg.prevChar = sReg.currChar; // Simpan huruf sebelumnya (pemeriksaan huruf jamak).
    }
    printSyll(buff, wReg.end,""); // Cetak suku kata di akhir kata (tanpa separator).
}

// Fungsi Utama Pemisah Suku Kata. Pengulangan untuk Setiap Karakter pada Teks.
// Parameter:
// - buff   <char*>   : Buffer penyimpanan teks.
// - len    <uint16_t>: Panjang teks.
void syllSplit(char *buff, uint16_t len){
    wReg.flag=0;
    for(uint16_t i = 0;i < len;i++){
        parseWord(buff, len, i);
        if(wReg.flag == SET) strideWord(buff);
    }
}

// Fungsi Utama Program.
int main() {
    char line[WLEN];
    uint16_t len = parseStr(line);
    syllSplit(line, len);
    return 0;
}