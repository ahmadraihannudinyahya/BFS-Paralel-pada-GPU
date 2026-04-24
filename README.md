# BFS-Paralel-pada-GPU

##  Deskripsi
Proyek ini berfokus pada implementasi algoritma **Breadth-First Search (BFS)** secara paralel menggunakan GPU. Tujuan utama dari proyek ini adalah untuk menganalisis bagaimana pemilihan **struktur data graph** mempengaruhi performa traversal pada arsitektur paralel.

![Visualisasi Representasi Graph](./visualization/Visualisasi_Representasi_Graph.png)


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

## Visualisasi Sistem

### 1. Flowchart Implementasi

Flowchart berikut menunjukkan perbedaan alur eksekusi BFS berdasarkan representasi graph dan model komputasi.

---

#### 1.1 Linked List

##### Sequential
![Visualisasi](./visualization/Flowchart/Sequence/linkedlist.png)

##### Paralel
![Visualisasi](./visualization/Flowchart/paralel/linkedlist.png)

**Insight:**
- Sulit diparalelkan karena pointer
- Tidak cocok untuk GPU

---

#### 1.2 CSR

##### Sequential
![Visualisasi](./visualization/Flowchart/Sequence/csr.png)

##### Paralel
![Visualisasi](./visualization/Flowchart/paralel/csr.png)

**Insight:**
- Akses memori terstruktur
- Optimal untuk GPU

---

#### 1.3 COO

##### Sequential
![Visualisasi](./visualization/Flowchart/Sequence/coo.png)

##### Paralel
![Visualisasi](./visualization/Flowchart/paralel/coo.png)

**Insight:**
- Mudah diparalelkan
- Kurang efisien dibanding CSR

### 2. Timeline Paralel
Visualisasi berikut menunjukkan bagaimana thread GPU bekerja pada setiap representasi graph.

#### Linked List
![Visualisasi](./visualization/Timeline/lingkedlist.png)

#### CSR
![Visualisasi](./visualization/Timeline/csr.png)

#### COO
![Visualisasi](./visualization/Timeline/coo.png)


### 3. Hasil Profiling
#### 3.1 Memcpy Total Bytes
![Visualisasi](./visualization/Profiling/MemcpyTotalBytes.png)

#### 3.2 Memcpy Total Time
![Visualisasi](./visualization/Profiling/KernelTotalTime.png)

#### 3.3 Kernel Total Time
![Visualisasi](./visualization/Profiling/KernelTotalTime.png)


**Analisis:**

- CSR menunjukkan performa terbaik
- Linked List paling lambat
- COO berada di tengah
