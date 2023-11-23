#include <stdio.h>
#include <inttypes.h>
#include <string.h>
		
// Periksa Batasan Array.
#define BOUND_CHECK(p,s) (p >= 0 && p < s)
// Periksa Karakter Alfabet.
#define IS_ALPHA(arr,s,p) (((arr[p] >= 'a' && arr[p] <= 'z') ||\
(arr[p] >= 'A' && arr[p] <= 'Z')) && BOUND_CHECK(p,s))
// Periksa Huruf Vokal.
#define IS_VOWEL(c) ((c == 'a')||(c == 'i')||(c == 'u')||(c == 'e')||(c == 'o')\
||(c == 'A')||(c == 'I')||(c == 'U')||(c == 'E')||(c == 'O'))
	
// Flag Set untuk Record.
#define SET 2
// Maksimum Karakter pada Teks.
#define WLEN 1234567890 //Maks. teoritis: 2147483647
		
// Definisi Tipe Data Splitter Record.
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
		
// Deklarasi Global Variabel Kerja & Fungsi.
		
// Splitter Record.
splt_t rec;
// Buffer Teks.
char line[WLEN];
		
// Parsing Data stdin ke Buffer. Maksimum Sepanjang Ukuran Buffer.
// Parameter:
// - buff <char*>: Array karakter untuk menyimpan data.
// Return Value:
// - <size_t>: Panjang string.
size_t parseStr(char *buff){
    printf("Masukan kalimat (maks. %u karakter):\n", WLEN-1);
    fgets(buff, WLEN, stdin);
    return strlen(buff);
}
		
// Parsing Kata dari Teks yang Diberikan. Simpan Data ke Record.
// Parameter:
// - buff <char*> : Buffer penyimpanan teks.
// - len  <size_t>: Panjang buffer (untuk bounding check).
// - i    <size_t>: Indeks pembacaan karakter pada teks.
void parseWord(char *buff, size_t len, size_t i){
    if(!IS_ALPHA(buff,len,i)){
    printf("%c", buff[i]);  // Cetak langsung karakter non alfabet.
    }else{
        if(!IS_ALPHA(buff,len,i-1) && IS_ALPHA(buff,len,i)) rec.begin=i,rec.flag++;
        if(!IS_ALPHA(buff,len,i+1) && IS_ALPHA(buff,len,i)) rec.end=i,rec.flag++;
    }   // Cari awal & akhir kata.
}
		
// Kembalikan Status Pencacah Huruf ke Nol. Simpan Perubahan ke Record.
void rstCount(){
    rec.sCons=0;rec.mCons=0;rec.vow=0;rec.prevChar = ' ';
}
		
// Cetak Setiap Karakter pada Suku Kata. Tambahkan Separator untuk Tengah Kata.
// Parameter:
// - buff <char*> : Buffer penyimpanan teks.
// - n    <size_t>: Indeks karakter terakhir pada suku kata.
// - add  <char*> : Separator suku kata (opsional).
void printSyll(char *buff, size_t n, char *add){
    for(size_t i = rec.begin; i <= n;i++) printf("%c", buff[i]);
    printf("%s", add);
}
		
// Parsing Suku Kata pada Kata. Operasikan Record.
// Parameter:
// - buff <char*>  : Buffer penyimpanan teks.
// - n    <size_t*>: Indeks pembacaan karakter pada kata.
void parseSyll(char *buff, size_t *n){
    switch(rec.sCons){
        case 0 : *n -= 1;break;
        case 1 : *n -= (1 + rec.sCons + rec.mCons);break;
        default: *n -= (1 + (rec.sCons - 1 - rec.mCons)+(2 * rec.mCons));break;
    }   // Ubah indeks mundur ke akhir suku kata.
    printSyll(buff, *n, "-");   // Cetak suku kata beserta separator.
    rec.begin=*n+1;rstCount();  // Karakter awal tepat setelah karakter akhir.
}
		
// Periksa Huruf Vokal beserta Diftong/Monoftong. Simpan Pencacahan ke Record.
void vowelCheck(){
    switch(rec.prevChar){
        case 'a':
        case 'e': if(rec.currChar != 'i' && rec.currChar != 'u') rec.vow++;break;
        case 'o': if(rec.currChar != 'i') rec.vow++;break;
        default : rec.vow++;
    }   // Jika diftong/monoftong ditemukan, cukup cacah sekali.
}
		
// Periksa Huruf Konsonan beserta konsonan jamak. Simpan Pencacahan ke Record.
void consonantCheck(){
    // Konsonan jamak sebelum pemisah suku kata tidak dihitung.
    if(rec.sCons == 1 && rec.mCons > 0) rec.mCons--;
    // Konsonan jamak dicacah untuk kalkulasi lokasi pemisah suku kata.
    switch(rec.prevChar){
        case 'n':(rec.currChar != 'y'&& rec.currChar != 'g') ? 
        rec.sCons++ : rec.mCons++;break;
        case 's':(rec.currChar != 'y') ? rec.sCons++ : rec.mCons++;break;
        case 'k':(rec.currChar != 'h') ? rec.sCons++ : rec.mCons++;break;
        default:rec.sCons++;
    }   // Konsonan jamak juga masuk ke konsonan tunggal untuk klasifikasi.
}
		
// Periksa Setiap Karakter pada Kata secara berurutan. Operasi Pemisah Suku Kata.
// Parameter:
// - buff <char*>: Buffer penyimpanan teks.
void strideWord(char *buff){
    rec.flag = 0;   // Reset flag word splitter.
    rstCount();     // Inisialisasi ulang record pemisah suku kata.
    for(size_t i = rec.begin;i <= rec.end;i++){     // Untuk setiap huruf pada kata.
        rec.currChar = buff[i];
        if(IS_VOWEL(rec.currChar)){     // Huruf vokal sebagai flag splitter suku kata.
            vowelCheck();
            if(rec.vow == SET) parseSyll(buff, &i); // Cetak suku kata yang ditemukan.
        }else if(rec.vow)               // Hanya baca konsonan setelah huruf vokal pertama.
            consonantCheck();
        rec.prevChar = rec.currChar;    // Simpan huruf sebelumnya (periksa huruf jamak).
    }
    printSyll(buff, rec.end,"");        // Cetak suku kata di akhir kata (tanpa separator).
}
		
// Fungsi Utama Pemisah Suku Kata. Pengulangan untuk Setiap Karakter pada Teks.
// Parameter:
// - buff   <char*> : Buffer penyimpanan teks.
// - len    <size_t>: Panjang teks.
void syllSplit(char *buff, size_t len){
    rec.flag=0;
    for(size_t i = 0;i < len;i++){
        parseWord(buff, len, i);
        if(rec.flag == SET) strideWord(buff);
    }
}
		
// Fungsi Utama Program.
// Return Value:
// - <int>: Exit code 0 jika program berjalan normal.
int main() {
    size_t len = parseStr(line);
    syllSplit(line, len);
    return 0;
}
