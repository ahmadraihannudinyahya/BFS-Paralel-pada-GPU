# BFS-Paralel-pada-GPU

##  Deskripsi
Proyek ini berfokus pada implementasi algoritma **Breadth-First Search (BFS)** secara paralel menggunakan GPU. Tujuan utama dari proyek ini adalah untuk menganalisis bagaimana pemilihan **struktur data graph** mempengaruhi performa traversal pada arsitektur paralel.

Tiga representasi graph yang dibandingkan dalam penelitian ini adalah:
- Edge List  
- CSR (Compressed Sparse Row)  
- COO (Coordinate Format)  

Dengan menggunakan dataset graph berukuran kecil hingga besar, proyek ini mengevaluasi efisiensi masing-masing representasi dalam konteks komputasi paralel.

---

## Overview

Traversal graph merupakan operasi fundamental dalam berbagai aplikasi seperti analisis jaringan sosial, sistem rekomendasi, dan pemrosesan data skala besar. Seiring meningkatnya ukuran data, pendekatan sekuensial menjadi kurang efisien, sehingga diperlukan komputasi paralel menggunakan GPU.

Dalam implementasi BFS paralel, performa tidak hanya ditentukan oleh algoritma, tetapi juga oleh cara data graph direpresentasikan di memori. Perbedaan pola akses memori pada setiap representasi dapat berdampak signifikan terhadap:

- Kecepatan eksekusi  
- Efisiensi penggunaan memori  
- Skalabilitas terhadap ukuran graph  

Proyek ini mengeksplorasi hubungan antara **struktur data graph** dan **kinerja BFS paralel**, dengan tujuan menemukan representasi yang paling optimal untuk digunakan pada GPU.

---

## Tujuan Utama
- Mengimplementasikan BFS paralel pada GPU  
- Membandingkan performa tiga representasi graph  
- Menganalisis dampak ukuran graph terhadap kinerja  
- Mengidentifikasi representasi paling efisien  
