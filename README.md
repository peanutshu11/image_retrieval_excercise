# 🖼️ Image Retrieval System

An image retrieval system that allows users to search for visually similar images based on multiple feature extraction techniques and clustering methods.

## 📌 Overview

This project implements a **Content-Based Image Retrieval (CBIR)** system using both global and local image features. The system supports:

- Feature extraction (color, edge, texture)
- Image clustering using K-means
- Fast image querying
- Evaluation using **Mean Average Precision (mAP)**

## 🛠️ Tech Stack

- **Language:** C++20
- **IDE:** Visual Studio 2022
- **UI Framework:** Qt 6.9.1
- **Libraries:**
  - OpenCV 4.1.0
  - Qt

## 🚀 Features

### 🔍 Feature Extraction
The system supports multiple image features:

- **Color Histogram (HSV)**
  - 2D histogram (Hue: 12 bins, Saturation: 6 bins → 72 dimensions)
  - Ignores Value channel for illumination robustness

- **Edge Orientation Histogram**
  - Uses Sobel operator
  - 18 bins (each represents 10°)

- **Color Correlogram**
  - Captures spatial color relationships
  - Uses Euclidean distance

- **Local Features**
  - SIFT (L2 distance)
  - ORB (Hamming distance)

---

### 📊 Distance Metrics

- Correlation-based distance (for histograms)
- Euclidean distance (L2)
- Hamming distance (for ORB)

---

### 📦 Clustering

- **K-means clustering**
- Each feature type has its own centroid computation:
  - Histogram-based: mean of bins
  - SIFT/ORB: clustering descriptors

---
### 🔎 Image Retrieval

- Select a collection
- Upload/query an image
- Retrieve top-N similar images
- Results sorted by similarity (most → least similar)
- Query time displayed

---

## 📈 Evaluation (mAP)

The system is evaluated using **Mean Average Precision (mAP)** on the [TMBuD dataset](https://github.com/CipiOrhei/TMBuD).

### Results:

| Feature              | Top-3 | Top-5 | Top-11 | Top-21 |
|---------------------|------|------|-------|-------|
| Color Histogram     | 0.197 | 0.222 | 0.243 | 0.248 |
| Edge Histogram      | 0.158 | 0.162 | 0.165 | 0.170 |
| Color Correlogram   | 0.194 | 0.201 | 0.213 | 0.225 |

> ⚠️ Note: SIFT and ORB were not evaluated due to high computational cost.

---

## ⚙️ Installation & Setup

### Requirements

- Windows OS (recommended)
- Visual Studio 2022
- Qt 6.9.1
- OpenCV 4.1.0

### Download Executable

Due to large size, download the compiled program here:

https://drive.google.com/drive/folders/1Hao2LBPCIzOFbIehJNe9nT3S7UCfHo-w

> Ensure the `.exe` file is placed in the correct directory structure.

---

## 🧪 Dataset Preparation

1. Download dataset:  
   https://github.com/CipiOrhei/TMBuD

2. Use provided script:
```bash
python prepare_data.py
```