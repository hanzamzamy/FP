# Deskripsi
Dalam pembelajaran bahasa dan sastra, pemahaman tentang suku kata sangat penting. Program yang akan
dibuat harus mampu membagi kalimat dalam bahasa Indonesia menjadi suku kata-suku kata yang sesuai.
# Spesifikasi
1. Program harus ditulis dalam bahasa pemrograman C.
2. Pengguna harus dapat memasukkan sebuah kalimat untuk diproses.
3. Program harus membagi kata menjadi suku kata sesuai aturan bahasa Indonesia.
4. Keluaran harus ditampilkan dengan setiap suku kata dipisahkan oleh tanda strip (-).
# Aturan Pembentukan Suku Kata
- Suku kata dimulai dengan vokal.
- Konsonan sebelum vokal dapat menjadi bagian dari suku kata berikutnya.
- Program harus mengikuti aturan pembentukan suku kata yang diberikan.
## Aturan Saklar
kv.v     2 0 1
kv.d(vv) 2 0 2
kvk.kv   2 2 2
v.kv     2 1 2
vk.kv    2 2 2
kv.kv    2 1 2
vk.kkv   2 3 3
vk.kkkv  2 4 4