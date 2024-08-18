# Design and Analysis of Algorithms - Programming Assignment 2024

This repository contains the solutions to the programming assignment for the Design and Analysis of Algorithms course (2024).

## Assignment Overview

The assignment consists of two tasks:

1. **Karatsuba Multiplication**
   - Implement a program to multiply two arbitrary long integers using the Karatsuba multiplication algorithm.
   - The input integers are provided in a file in hexadecimal format, and the output should also be in hexadecimal format.
   - The program takes the input and output file names as command-line arguments.

2. **K-Means Clustering using Minimum Spanning Tree (MST)**
   - Implement a clustering algorithm that uses the Minimum Spanning Tree (MST) to find k clusters in a set of points in \( \mathbb{R}^n \).
   - The program reads the number of data points, their dimensionality, and the value of k from an input file, and writes the clustered points along with their cluster labels to an output file.
   - The program takes the input and output file names as command-line arguments.

## File Structure

- *karatsuba.c*: The implementation of the Karatsuba multiplication algorithm.
- mst_kmeans.c: The implementation of the k-means clustering using MST.
- Makefile: Instructions to compile the programs.
- input_karatsuba.txt: Sample input file for the Karatsuba multiplication program.
- input_mst.txt: Sample input file for the k-means clustering program.
- output_karatsuba.txt: Sample output file for the Karatsuba multiplication program.
- output_mst.txt: Sample output file for the k-means clustering program.

