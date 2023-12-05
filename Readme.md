# Deskripsi Program

Program yang dibuat untuk _Final Project_ ini merupakan program yang ditulis menggunakan bahasa pemrograman C 
yang mampu menerima _input_ teks dan memisah setiap kata menjadi suku kata. Suku kata dipisahkan oleh tanda 
`-` dan mengikuti kaidah bahasa Indonesia. Adapun, acuan yang mendasari program ini adalah Ejaan Bahasa Indonesia 
yang Disempurnakan (EYD) Edisi ke-5. Hal ini menyesuaikan ketetapan yang berlaku di mana EYD V resmi menggantikan Pedoman 
Umum Ejaan Bahasa Indonesia (PUEBI) sejak 16 Agustus 2022 berdasarkan Surat Keputusan Kepala Badan Pengembangan dan 
Pembinaan Bahasa Kemendikbudristek Republik Indonesia No. 0424/I/BS.00.01/2022. Program ini tidak memiliki kapabilitas 
untuk mengidentifikasi dan mengklasifikasikan kata secara lingusitik sehingga setiap kata dianggap sebagai kata dasar. 
Aturan pemenggalan kata dasar menurut EYD V dapat dilihat pada lampiran surat keputusan, bab kedua, subbab ketiga, poin 
pertama tentang pemenggalan kata dasar.[1]

# Struktur Program

## Preprosesor
Pada bagian awal kode program (`line 1-17`), terdapat sekumpulan _preprocessor_, yaitu `#include` dan `#define`. `#include` 
digunakan untuk memasukan _library_ yang digunakan pada program.

- `stdio.h` untuk fungsi I/O seperti `printf()` dan `fgets()`.
- `inttypes.h` untuk definisi tipe data _integer_ seperti `uint8_t` dan `uint32_t`.
- `string.h` untuk fungsi _string_ seperti `strlen()`.

`#define` digunakan untuk mendefinisikan _macro_ dan konstanta yang digunakan pada program. _Macro_ yang digunakan 
semuanya berupa _comparison operation_ dengan _output_ berupa _boolean logic_.

- `BOUND_CHECK(p,s)` untuk memeriksa apakah `p` merupakan indeks yang valid untuk _array_ berukuran `s`.
- `IS_ALPHA(arr,s,p)` untuk memeriksa apakah karakter berindeks `p` pada _array_ `arr` yang berukuran `s` adalah huruf 
alfabet dan berada di dalam _array_.
- `IS_VOWEL(c)` untuk memeriksa apakah karakter `c` merupakan huruf vokal.
- `SET` sebagai indikator operasi pemecahan.
- `WLEN` sebagai ukuran _buffer_ atau panjang maksimum teks.

Nilai maksimum `WLEN` secara teori adalah 2<sup>31</sup>-1 akibat limitasi fungsi `fgets()`. Nilai pada praktiknya 
kembali kepada kapasitas memori perangkat—_buffer_ dengan ukuran maksimum membutuhkan memori ±`2.1GB` dan belum termasuk 
komponen lain yang perlu alokasi memori. Khusus Windows, program harus di-_compile_ sebagai aplikasi 64 bit (`x64`), 
karena batasan _single process executable program_ 32 bit (`Win32`) hanya boleh mengakses memori sebesar `2GB`. OS 
berbasis UNIX tidak memiliki kendala demikian untuk aplikasi 32 bit (limit: `3GB`)—belum dilakukan percobaan untuk MacOS.

## Struktur Data
Bagian selanjutnya (`line 18-32`) adalah definisi struktur sekaligus tipe data baru. Tipe data `splt_t` menghimpun semua 
komponen yang diperlukan program dalam satu variabel. _Member_ dari struktur ini:

- `begin` & `end` untuk menyimpan indeks karakter awal & akhir kata.
- `flag` untuk menyimpan status operasi pemisahan kata.
- `sCons`, `vow`, dan `mCons` untuk mencacah huruf konsonan tunggal, vokal, dan konsonan jamak.
- `prevChar` & `currChar` untuk menyimpan karakter sebelum dan yang sedang diproses.

Semua tipe data _member_ struktur adalah _unsigned integer_. Alasan tipe data yang digunakan merupakan definisi dari 
_library_ `inttypes.h` karena nomenklatur tipe datanya lebih jelas tidak bergantung pada target _compiler_. Struktur ini 
juga menerapkan _bit fields_, yaitu membatasi lebar bit _member_ guna meningkatkan efisiensi ukuran struktur. Efisiensi 
ditingkatkan tanpa menggunakan _preprocessor_ `#pragma pack()`, tetapi dengan meminimalisir _padding_ sehingga data tetap 
_aligned_—catatan: struktur ini sangat padat & efisien dengan _padding_ sebesar `0`. Berikut merupakan representasi 
bagaimana data pada struktur ini terdistribusi.

|            Byte 1            |           Byte 2            |      Byte 3       |      Byte 4       |
|:----------------------------:|:---------------------------:|:-----------------:|:-----------------:|
|        `begin`[8 bit]        |       `begin`[8 bit]        |  `begin`[8 bit]   |  `begin`[8 bit]   |
|         `end`[8 bit]         |        `end`[8 bit]         |   `end`[8 bit]    |   `end`[8 bit]    |
| `flag`[2 bit]+`sCons`[6 bit] | `vow`[2 bit]+`mCons`[6 bit] | `prevChar`[8 bit] | `currChar`[8 bit] |
|            Total             |            `12B`            |     _Padding_     |        `0`        |

Jika tidak menggunakan _bit fields_, hanya menggunakan tipe data terkecil untuk _member_ selain `begin` & `end`, maka 
ukurannya adalah `16B` (`14B`+`2B` _padding_). Ukuran struktur sebenarnya bisa diperkecil lagi dengan mengubah tipe data 
_member_ `begin` & `end` menjadi lebih kecil—konsekuensinya adalah maksimum `WLEN` ikut mengecil. Misalnya, `uint16_t` 
(`WLEN` maks. 2<sup>16</sup>-1): `8B`; `uint8_t` (`WLEN` maks. 2<sup>8</sup>-1): `6B`.

## Variabel Global & Fungsi
Bagian selanjutnya (`line 33-156`) adalah deklarasi variabel global dan fungsi. Variabel kerja pada program:

- `rec` (`splt_t`) merupakan _record_ berisi variabel-variabel kerja yang disatukan  dalam satu struktur.
- `line` (`char[]`) merupakan _buffer_ berukuran `WLEN` di mana karakter-karakter teks _input_ disimpan.

Alasan kedua variabel tersebut dijadikan variabel global karena variabel global memiliki _scope_ yang luas sehingga 
variabel tersebut dapat diakses dari dalam fungsi manapun pada program. Variabel global tidak seperti variabel lokal yang 
dihapus setelah program keluar dari _function_/_loop body_ sehingga memiliki letak/alamat yang tetap pada memori, yang 
artinya akses ke datanya lebih optimal & cepat karena alamatnya sama. Selain itu, karena variabel lokal disimpan pada _stack_, 
maka ukurannya terbatas hanya `1-2MB`—walaupun bisa di-_tweak_ dengan mengubah _default stack size_ pada _compiler_.
Program ini melakukan operasi yang cukup kompleks sehingga praktik yang baik adalah dengan memecah program menjadi 
subprogram dengan operasi sederhana. Di sini terdapat 9 fungsi (tidak termasuk `main()`):

- `parseStr()` untuk mengambil _input_ dan menyimpannya pada _buffer_, serta mengembalikan  panjang _string_ yang terbaca.
- `parseWord()` untuk memecah teks pada _buffer_ menjadi kata, untuk diproses lebih lanjut oleh operasi pemecah suku kata, dengan mencari karakter awal & akhir kata.
- `rstCount()` untuk mengembalikan nilai pencacah huruf ke nilai awal.
- `printSyll()` untuk mencetak suku kata berdasarkan indeks karakter awal & akhir suku kata yang diberikan, serta mencetak karakter pemisah suku kata `-` bila diperlukan.
- `parseSyll()` untuk menentukan indeks karakter awal & akhir suku kata berdasarkan data pada _counter_ huruf sekaligus mencetaknya.
- `vowelCheck()` untuk memeriksa huruf vokal tunggal, diftong, dan monoftong, serta mencacahnya.
- `consonantCheck()` untuk memeriksa huruf konsonan tunggal dan jamak, serta mencacahnya.
- `strideWord()` untuk memecah kata menjadi suku kata dengan mencari pola berdasarkan pencacahan huruf vokal dan konsonan.
- `syllSplit()` untuk memecah teks pada _buffer_ menjadi suku kata yang tercetak dengan melakukan operasi pemecahan kata dan pemecahan suku kata.

Setiap fungsi selanjutnya akan dijelaskan terpisah secara mendetail. Urutan penjelasan menyesuaikan urutan fungsi tersebut 
dipanggil, mulai dari fungsi yang paling pertama dipanggil.

# Deskripsi Fungsi

## Fungsi parseStr()
Fungsi ini digunakan untuk mengambil _input_ dari _user_ dan menyimpannya pada _buffer_ karakter. Selain itu, fungsi ini 
juga mengembalikan panjang _string_ yang dibacanya. Tipe data fungsi ini adalah `size_t` yaitu _unsigned integer_ yang 
biasa digunakan sebagai _iterator_. Perintah `fgets(buff, WLEN, stdin)` membaca karakter dari _stream_ `stdin` dan 
menyalinnya ke _buffer_ `buff`. Fungsi `fgets()` membaca hingga `WLEN`-1, menyisakan 1 karakter untuk _null terminator_ (`\0`), 
sehingga _buffer_ tidak _overflow_. Perintah `strlen(buff)` menghitung jumlah karakter pada `buff` mulai dari karakter 
pertama hingga karakter sebelum _null terminator_.

## Fungsi syllSplit()
Fungsi ini digunakan sebagai _wrapper_ dari operasi pemisahan suku kata pada teks. Karena logika operasi ini cukup kompleks, 
maka operasinya dipecah menjadi operasi penentu kata (`parseWord()`) dan operasi penentu suku kata (`strideWord()`). Fungsi 
dimulai dengan me-_reset_ _flag_ operasi pemisah kata pada _record_ `rec`. Kemudian, iterasi dilakukan untuk setiap karakter 
di _buffer_. Ketika operasi pemisahan kata `parseWord()` berhasil (satu kata ditemukan), nilai `rec.flag` berubah menjadi 
`SET`. Ketika kata ditemukan, operasi pemisah suku kata dilakukan menggunakan fungsi `strideWord()`. Siklus berulang 
(mencari kata & memecah suku kata) hingga _buffer_ habis teriterasi.

## Fungsi parseWord()
Fungsi ini digunakan untuk mencari kata pada _buffer_ teks dengan menentukan indeks karakter pertama dan terakhir dari kata 
tersebut pada _buffer_. Operasi dilakukan dengan mengidentifikasi huruf alfabet pada _buffer_, karakter demi karakter. Ada 
dua percabangan: jika karakter bukan huruf alfabet, cetak karakter; atau jika karakter merupakan huruf alfabet, periksa 
kemungkinan karakter tersebut sebagai awalan & akhiran kata. Awalan kata ditandai dengan huruf alfabet yang didahului oleh 
karakter non-alfabet, sementara akhiran kata ditandai dengan huruf alfabet yang diikuti dengan karakter non-alfabet. Ketika 
karakter awal atau akhir kata ditemukan, indeks karakter disimpan pada _member_ `rec` yang sesuai dan nilai `rec.flag` 
ditambahkan. Ketika `rec.flag` mencapai nilai 2 / `SET`, satu kata ditemukan.

## Fungsi strideWord()
Fungsi ini digunakan untuk memisah kata menjadi suku kata. Operasi pemisahan suku kata dilakukan kata per kata. Pemisahan 
suku kata dilakukan dengan mengidentifikasi pola huruf vokal & konsonan. Berikut pola umum pemisahan kata dasar sesuai EYD V. 
Catatan: (v)okal, (k)onsonan.

- Vokal beruntun: bu-ah (`kv-vk`).
- Konsonan diapit vokal: sau-da-ra (`kvv-kv-kv`); ba-pak (`kv-kvk`); mu-ta-khir (`kv-kv-kkvk`).
- Konsonan beruntun diapit vokal: man-di (`kvk-kv`); in-stru-men (`vk-kkkv-kvk`); makh-luk (`kvkk-kvk`).

Berdasarkan pola-pola tersebut, algoritma yang diimplementasikan untuk memisah suku kata adalah

1. Iterasi karakter mulai dari awal hingga akhir kata untuk menentukan huruf vokal & konsonan.
2. Setiap iterasi menyimpan karakter yang diperiksa dan karakter yang diperiksa sebelumnya sebagai mekanisme pemeriksaan diftong, monoftong, dan konsonan jamak.
3. Huruf vokal dicacah. Diftong & monoftong hanya dihitung satu.
4. Jika dua huruf vokal ditemukan, maka ada pemisahan suku kata (posisi menyesuaikan huruf konsonan).
5. Hanya huruf konsonan setelah huruf vokal pertama yang dicacah. Pencacah konsonan jamak terpisah, tetapi konsonan jamak juga dicacah oleh pencacah konsonan (dihitung satu).
6. Jika huruf konsonan pertama merupakan konsonan jamak, konsonan tersebut tidak perlu dicacah oleh pencacah konsonan jamak. Berlaku jika ditemukan >1 konsonan.
7. Posisi huruf terakhir suku kata dihitung mundur dari indeks iterasi saat ini. Untuk semua kasus, indeks dikurangi 1 (huruf vokal terakhir terbaca).
8. Kasus konsonan=1, konsonan dapat berupa tunggal atau jamak (2 huruf). Indeks dikurangi nilai cacah konsonan dan konsonan jamak—karena konsonan jamak juga dicacah oleh pencacah konsonan biasa.
9. Kasus konsonan $>1$, distribusi konsonan tunggal & jamak bisa beragam. Indeks dikurangi nilai cacah (konsonan-1-konsonan jamak) dan dua kali nilai konsonan jamak. Pastikan konsonan jamak yang merupakan konsonan pertama tidak dihitung pencacah konsonan jamak.
10. Setelah karakter awal dan akhir suku kata ditentukan, cetak semua karakter pada \textit{range} tersebut. Tambahkan karakter pemisah `-`.
11. Karakter awal suku kata berikutnya adalah karakter akhir suku kata +1.
12. Suku kata terakhir biasanya tidak memenuhi syarat huruf vokal berjumlah 2 dan tidak dicetak secara otomatis oleh algoritma. Cetak suku kata terakhir secara manual tanpa pemisah `-`.

Fungsi ini dimulai dengan me-_reset_ nilai _member_ pada _record_ `rec` seperti `flag`, `prevChar`, dan seluruh _counter_
untuk menandakan awal dari operasi pemisahan baru. Selanjutnya, iterasi untuk semua karakter pada kata tersebut dilakukan 
sesuai algoritma yang dijelaskan sebelumnya. Operasi ini dipecah menjadi beberapa subproses untuk pencacahan huruf vokal 
(`vowelCheck()`) & konsonan (`consonantCheck()`), penentuan indeks karakter akhir suku kata (`parseSyll()`), dan pencetakan 
suku kata beserta pemisahnya (`printSyll()`).

## Fungsi rstCount()
Fungsi ini digunakan untuk mengembalikan nilai _counter_ dan data karakter terakhir yang diperiksa ke nilai awalnya (`0` dan `' '`). 
Fungsi ini memastikan operasi pencacahan dilakukan tanpa kesalahan dengan me-_reset_ nilai pencacah sebelum memulai operasi baru.

## Fungsi vowelCheck()
Fungsi ini digunakan pada operasi pemisahan suku kata untuk memeriksa keberadaan dan mencacah jumlah huruf vokal pada kata. 
Terdapat kemungkinan adanya diftong & monoftong pada kata, sementara diftong & monoftong merupakan satu kesatuan menurut EYD V. 
Untuk mencegah misidentifikasi diftong & monoftong sebagai huruf vokal (terpisah) beruntun, maka perlu dilakukan pemeriksaan 
terhadap karakter sebelumnya karena diftong & monoftong terdiri atas 2 huruf. Percabangan `switch` dan `if` digunakan untuk 
memeriksa sekuensial diftong & monoftong. Huruf vokal pertama pasti terhitung oleh _counter_ huruf vokal sehingga huruf vokal 
setelahnya tidak perlu dicacah jika memenuhi sekuensial diftong atau monoftong.

## Fungsi consonantCheck()
Fungsi ini digunakan untuk memeriksa keberadaan dan mencacah jumlah huruf konsonan pada kata. Menurut EYD V, terdapat konsonan 
jamak yang memang tidak dipisah seperti _ng_, _ny_, _sy_, dan _kh_. Supaya tidak terjadi misidentifikasi konsonan tersebut 
sebagai 2 konsonan terpisah, mekanisme yang sama seperti pada fungsi `vowelCheck()` diterapkan. Konsonan jamak cukup dicacah 
satu kali oleh _counter_ konsonan, sama seperti kasus diftong & monoftong, dan jumlah huruf konsonan jamak yang ditemukan juga 
dicacah pada _counter_ tersendiri. _Counter_ konsonan digunakan untuk mengidentifikasi pola seperti `vk-kv`, `v-kv`, `v-v`, 
sementara _counter_ konsonan jamak digunakan untuk menentukan posisi aktual pemisah karena posisi dihitung dari indeks pembacaan 
karakter terakhir. Contoh: kata [b]a-pa[k] dan [b]a-nya[k] sama-sama memiliki pola `v-kv` (huruf di kurung siku tidak dihitung), 
tetapi jumlah huruf setelah pemisahnya berbeda. Selain itu, konsonan jamak yang merupakan konsonan pertama suku kata dengan banyak 
konsonan tidak dihitung. Misal: [m]akh-lu[k] memiliki vokal=2, konsonan=2, dan jamak=0. Hal ini mempermudah penentuan indeks dari belakang.

## Fungsi parseSyll()
Fungsi ini digunakan untuk menentukan indeks huruf terakhir suku kata, mencetak suku kata tersebut beserta pemisahnya, 
dan memperbarui indeks awal untuk suku kata selanjutnya. Indeks karakter akhir suku kata menggunakan acuan karakter 
terakhir yang diperiksa (indeks mundur). Alasan menggunakan _reverse indexing_ adalah karena huruf konsonan sebelum huruf 
vokal pertama tidak dicacah. Untuk melakukan _normal indexing_, konsonan sebelum dan konsonan di antara huruf vokal 
(termasuk kemungkinan konsonan jamak) perlu dicacah sehingga menambah variabel kerja (_counter_). Contoh: in-struk-si, 
pada suku kata kedua dan ketiga. _Normal indexing_ perlu mencacah 'str' untuk _indexing_ dan 'uksi' untuk _pattern identification_. 
Di sisi lain, _reverse indexing_ hanya perlu mencacah 'uksi' untuk _indexing_ & _pattern identification_. Posisi karakter 
terakhir ditentukan berdasarkan jumlah huruf konsonan dan konsonan jamak (Lihat bagian 3.3.4 poin 7-9). Setelah indeks karakter 
terakhir ditentukan, fungsi `printSyll()` dipanggil untuk mencetak suku kata sekaligus pemisahnya (`-`). Terakhir, fungsi ini 
memperbarui indeks karakter awal untuk suku kata berikutnya dan memanggil `rstCount()`. Indeks karakter terakhir disimpan pada 
variabel _iterator_ pemisah suku kata, sehingga pembacaan karakter mundur kembali dari karakter awal suku kata berikutnya.

## Fungsi printSyll()
Fungsi ini digunakan untuk mencetak suku kata berdasarkan indeks-indeks karakter yang diberikan. Fungsi ini juga mencetak 
karakter tambahan seperti  pemisah suku kata (`-`) jika diperlukan.

# Limitasi

Walaupun program ini sudah mampu memenuhi aturan dan spesifikasi yang diminta, masih terdapat kemungkinan adanya celah yang 
menyebabkan _bug_ yang tidak terduga. Selain _bug_ tersebut, program ini juga memiliki batasan karena dibuat sesederhana mungkin 
tetapi tetap meng-_cover_ kemungkinan terburuk sebanyak-banyaknya. Salah satu limitasi yang patut diakui adalah ketidakmampuan 
program untuk mengidentifikasikan kata secara linguistik, hanya berdasarkan pola. Sedangkan dalam bahasa sendiri terdapat kata-kata 
yang memang tidak bisa disamakan polanya (_irregular_). Program ini juga tidak didesain untuk meng-_handle_ kata turunan, seperti 
kata berimbuhan, dan kata serapan karena polanya bisa beragam. Namun, masih ada beberapa imbuhan yang pola pemisahan katanya sama 
seperti pola pemisahan kata dasar sehingga program ini mampu memisah kata tersebut secara akurat. Sebagai contoh kasus, kata 
be-rang-kat (bukan turunan ber+angkat) menghasilkan _output_ yang sesuai: be-rang-kat; kata ber-u-bah (kata turunan ber+ubah) 
menghasilkan _output_ yang salah: be-ru-bah. 

Pendekatan identifikasi pola berdasarkan huruf vokal & konsonan memang tidak tepat untuk kata-kata kompleks. Untuk kata-kata 
kompleks diperlukan program yang memiliki kemampuan linguistik sehingga dapat mengidentifikasi kelas kata 
(verba, nomina, pronomina, adjektiva, adverbia), imbuhan, dan unsur-unsur kebahasaan lainnya. Identifikasi konteks juga penting 
untuk menghasilkan _output_ yang akurat, seperti kata _beruang_ yang bisa jadi ber-u-ang atau be-ru-ang.[2] 
Untuk tujuan yang kompleks ini, pendekatan yang sesuai adalah _Natural Language Processing_ (NLP).

# Referensi

[1] Badan Pengembangan dan Pembinaan Bahasa, (2022), _Keputusan Kepala Badan Pengembangan dan Pembinaan Bahasa Kementerian Pendidikan, Kebudayaan, Riset, dan Teknologi Republik Indonesia Nomor 0424/I/BS.00.01/2022 tentang Ejaan Bahasa Indonesia yang Disempurnakan_, Kemendikbudristek, Jakarta, https://badanbahasa.kemdikbud.go.id/resource/doc/files/SK_EYD_Edisi_V_16082022.pdf

[2] Badan Pengembangan dan Pembinaan Bahasa, (n.d.), _Kamus Besar Bahasa Indonesia Edisi ke-6 Daring_, Kemendikbudristek, http://kbbi.kemdikbud.go.id/
