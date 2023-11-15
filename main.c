#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Periksa Batasan Array.
#define BOUND_CHECK(p,s) (p >= 0 && p < s)
// Periksa Karakter Alfabet.
#define IS_ALPHA(arr,s,p) (((arr[p] >= 'a' && arr[p] <= 'z') ||\
(arr[p] >= 'A' && arr[p] <= 'Z')) && BOUND_CHECK(p,s))
// Periksa Huruf Vokal.
#define IS_VOWEL(c) ((c == 'a')||(c == 'i')||(c == 'u')||(c == 'e')||(c == 'o')\
||(c == 'A')||(c == 'I')||(c == 'U')||(c == 'E')||(c == 'O'))
// Flag Set untuk Register.
#define SET 2
// Maksimum Karakter pada Teks.
#define WLEN 1996379264

// Definisi Tipe Data Register Word Splitter.
// Member:
// - begin    <uint32_t>: Posisi karakter awal kata.
// - end      <uint32_t>: Posisi karakter akhir kata.
// - flag     <uint8_t> : Status operasi pemecah kata.
// - sCons    <uint8_t> : Jumlah huruf konsonan tunggal & jamak.
// - vow      <uint8_t> : Jumlah huruf vokal (sebagai flag).
// - mCons    <uint8_t> : Jumlah huruf konsonan jamak.
// - prevChar <uint8_t> : Karakter yang sebelumnya dibaca.
// - currChar <uint8_t> : Karakter yang dibaca sekarang.
typedef struct {
    uint32_t begin,end;
    uint8_t flag:2,sCons:6,vow:2,mCons:6,prevChar:8,currChar:8;
}splt_t;

// Variabel Kerja Global
splt_t reg;
char line[WLEN];

// Parsing Data stdin ke Buffer. Ubah Semua Karakter ke Lowercase.
// Parameter:
// - buff <char*>: Array karakter untuk menyimpan data.
// Return Value:
// - len <size_t>: Panjang string (termasuk NULL terminator).
size_t parseStr(char *buff){
    printf("Masukan kalimat (maks. %u karakter):\n", WLEN-1);
    fgets(buff, WLEN, stdin);
    return strlen(buff);
}

// Parsing Kata dari Teks yang Diberikan. Simpan Data ke Register.
// Parameter:
// - buff <char*>   : Buffer penyimpanan teks.
// - len  <size_t>: Panjang buffer (untuk bounding check).
// - i    <size_t>: Indeks pembacaan karakter pada teks.
void parseWord(char *buff, size_t len, size_t i){
    if(!IS_ALPHA(buff,len,i)){
        printf("%c", buff[i]);  // Cetak langsung karakter non alfabet.
    }else{
        if(!IS_ALPHA(buff,len,i-1) && IS_ALPHA(buff,len,i)) reg.begin=i,reg.flag++;
        if(!IS_ALPHA(buff,len,i+1) && IS_ALPHA(buff,len,i)) reg.end=i,reg.flag++;
    }   // Cari awal & akhir kata.
}

// Kembalikan Status Pencacah Huruf ke Nol. Simpan Perubahan ke Register.
void rstCount(){
    reg.sCons=0;reg.mCons=0;reg.vow=0;reg.prevChar = ' ';
}

// Cetak Setiap Karakter pada Suku Kata. Tambahkan Separator untuk Tengah Kata.
// Parameter:
// - buff <char*>   : Buffer penyimpanan teks.
// - n    <size_t>: Indeks karakter terakhir pada suku kata.
// - add  <char*>   : Separator suku kata (opsional).
void printSyll(char *buff, size_t n, char *add){
    for(size_t i = reg.begin; i <= n;i++)printf("%c", buff[i]);
    printf("%s", add);
}

// Parsing Suku Kata pada Kata. Operasi Register.
// Parameter:
// - buff <char*>    : Buffer penyimpanan teks.
// - n    <size_t*>: Indeks pembacaan karakter pada kata.
void parseSyll(char *buff, size_t *n){
    switch(reg.sCons){
        case 0 : *n -= 1;break;
        case 1 : *n -= (1 + reg.sCons + reg.mCons);break;
        default: *n -= (1 + (reg.sCons - 1 - reg.mCons)+(2 * reg.mCons));break;
    }   // Ubah indeks mundur ke akhir suku kata.
    printSyll(buff, *n, "-");   // Cetak suku kata beserta separator.
    reg.begin=*n+1;rstCount(); // Karakter awal tepat setelah karakter akhir.
}

// Periksa Huruf Vokal beserta Diftong/Monoftong. Simpan Pencacahan ke Register.
void vowelCheck(){
    switch(reg.prevChar){
        case 'a': case 'e': if(reg.currChar != 'i'&& reg.currChar != 'u') reg.vow++;break;
        case 'o': if(reg.currChar != 'i') reg.vow++;break;
        default : reg.vow++;
    }   // Jika diftong/monoftong ditemukan, cukup cacah sekali.
}

// Periksa Huruf Konsonan beserta konsonan jamak. Simpan Pencacahan ke Register.
void consonantCheck(){
    // Konsonan jamak sebelum pemisah suku kata tidak dihitung.
    if(reg.sCons == 1 && reg.mCons > 0) reg.mCons--;
    switch(reg.prevChar){
        case 'n':(reg.currChar != 'y'&& reg.currChar != 'g') ?
                 reg.sCons++ : reg.mCons++;break;
        case 's':(reg.currChar != 'y') ? reg.sCons++ : reg.mCons++;break;
        case 'k':(reg.currChar != 'h') ? reg.sCons++ : reg.mCons++;break;
        default:reg.sCons++; // Konsonan jamak masuk ke konsonan tunggal untuk klasifikasi.
    }   // Data terpisah konsonan jamak untuk kalkulasi lokasi pemisah suku kata.
}

// Periksa Setiap Karakter pada Kata secara berurutan. Operasi Penuh Pemisah Suku Kata.
// Parameter:
// - buff <char*>: Buffer penyimpanan teks.
void strideWord(char *buff){
    reg.flag = 0;  // Reset flag word splitter.
    rstCount();     // Inisialisasi register syllable splitter.
    for(size_t i = reg.begin;i <= reg.end;i++){ // Untuk setiap huruf pada kata.
        reg.currChar = buff[i];
        if(IS_VOWEL(reg.currChar)){   // Huruf vokal sebagai flag syllable splitter.
            vowelCheck();
            if(reg.vow == SET) parseSyll(buff, &i); // Cetak suku kata yang ditemukan.
        }else if(reg.vow)             // Hanya baca konsonan setelah huruf vokal pertama.
            consonantCheck();
        reg.prevChar = reg.currChar; // Simpan huruf sebelumnya (pemeriksaan huruf jamak).
    }
    printSyll(buff, reg.end,""); // Cetak suku kata di akhir kata (tanpa separator).
}

// Fungsi Utama Pemisah Suku Kata. Pengulangan untuk Setiap Karakter pada Teks.
// Parameter:
// - buff   <char*>   : Buffer penyimpanan teks.
// - len    <size_t>: Panjang teks.
void syllSplit(char *buff, size_t len){
    reg.flag=0;
    for(size_t i = 0;i < len;i++){
        parseWord(buff, len, i);
        if(reg.flag == SET) strideWord(buff);
    }
}

// Fungsi Utama Program.
int main() {
    printf("%llu", sizeof(splt_t));
    size_t len = parseStr(line);
    syllSplit(line, len);
    return 0;
}