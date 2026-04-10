# Learning Wiki: Saka Note

Selamat datang di jurnal pembelajaran Saka Note! Di sini kita mencatat evolusi arsitektur dan kebijaksanaan rekayasa yang kita temukan.

## 🏙️ Arsitektur (Peta Kota)

Saka Note dibangun dengan struktur **Single Activity, Multi-Composable**.
- **MainActivity**: Stasiun pusat yang memulai segalanya.
- **EditorApp.kt**: Jantung dari aplikasi, tempat di mana `TechnicalStandardNoteMobileApp` mengatur orkestrasi editor, tab, dan sistem keamanan.
- **Root Box Pattern**: Kita menggunakan satu `Box` besar sebagai root untuk memungkinkan layering (seperti overlay App Lock dan feedback loading) di atas UI utama.
- **Recursive Tree**: Struktur folder menggunakan pola *Adjacency List* di database (parentId) yang dirender secara rekursif di UI menggunakan `RecursiveFolderList`. Analogi: "Aplikasi ini seperti akar pohon yang terus bercabang ke bawah tanpa batas."

## 🛠️ Keputusan Teknis (Alasan Mengapa)

- **Jetpack Compose**: Kita memilih pendekatan deklaratif karena memudahkan manajemen state yang kompleks seperti sinkronisasi tab dan App Lock.
- **Custom Font Loading**: Kita mendukung font eksternal (.ttf/.otf) untuk memberikan kebebasan tipografi kepada pengguna, yang kita simpan secara aman menggunakan Android Scoped Storage.
- **UUID Identifiers**: Kita beralih dari ID bertipe `Long` ke `String` (UUID) untuk memastikan integritas data yang lebih baik saat melakukan operasi sinkronisasi dan identifikasi unik di struktur folder yang dalam.
- **Adjacency List + CTE**: Kita menggunakan SQLite *Recursive CTE* (`WITH RECURSIVE`) untuk mengambil seluruh hierarki folder dalam satu query tunggal.

## 🐛 Cerita Perang (Bug & Perbaikan)

### Log: 2026-04-11 - Kasus "Brace yang Hilang"
- **Masalah**: Build gagal total pada tahap `kaptGenerateStubs` dengan pesan error `Expecting '}'` di baris terakhir file (2999) dan `Unresolved Reference` untuk `zIndex`.
- **Penyebab**: Ternyata ada satu brace `}` yang lupa ditutup pada `Column` utama di tengah file, dan import `zIndex` berada di package yang salah.
- **Pelajaran**: Kapt sangat sensitif. Jika build gagal di baris terakhir file, periksa balance brace (`{}`) di seluruh file, terutama jika baru saja melakukan refactor besar pada fungsi root.

### Log: 2026-04-11 - Stabilisasi Build & Memori
- **Masalah**: JVM Crash (`hs_err_pid`) dan build gagal dengan `Unresolved reference: Folder` setelah refactor besar.
- **Penyebab**: Mesin host kehabisan memori (Paging File Exhausted) dan `Folder` icon ternyata berada di modul `material-icons-extended` yang belum diimpor (M3 BOM tidak memasukkannya secara default).
- **Pelajaran**: (1) Saat memori kritis, limit eksekusi Gradle (`parallel=false`, `workers=1`). (2) Jangan asumsikan semua icon Material tersedia di *core set*; icon kategori `Rounded` sering kali hanya ada di *extended set*.

## 🦉 Kebijaksanaan (Pelajaran Berharga)

- **Layering Matters**: Selalu tempatkan overlay (seperti lock screen) di urutan terakhir di dalam `Box` root agar mereka memiliki z-index visual tertinggi secara implisit.
- **Surgical Edit**: Saat memperbaiki build failure, lakukan perubahan seminimal mungkin untuk menghindari "comprehension debt" di masa depan.

## 💎 Praktik Terbaik

- **Import Precision**: Selalu gunakan path import yang spesifik (misalnya `androidx.compose.ui.draw.zIndex`) daripada import wildcard jika memungkinkan untuk mempercepat build.
- **State Hoisting**: State keamanan (`appUnlocked`, `appLockEnabled`) harus di-hoist ke level teratas agar bisa mengontrol seluruh UI secara konsisten.
