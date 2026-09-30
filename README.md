 
# LSB Image Steganography in C

## 📌 Project Overview

This project implements **LSB (Least Significant Bit) Image Steganography** using the C programming language.

Steganography is a technique used to hide secret information inside an image without significantly changing its appearance.

This project allows users to encode secret text into a BMP image and decode the hidden message from the encoded image.

## 🎯 Objectives

- To understand the concept of LSB steganography.
- To implement encoding and decoding using C.
- To hide secret text inside BMP images.
- To retrieve hidden information from stego images.
- To understand file handling, bitwise operations, and structures in C.

## 🛠️ Technologies Used

- **Programming Language:** C
- **Image Format:** BMP
- **Compiler:** GCC
- **IDE:** Visual Studio Code
- **Version Control:** Git and GitHub
- **Operating System:** Linux / Windows (with GCC)

## 📂 Project Structure

```text
LSB-Image-Steganography/
│
├── 3-Design/
│   └── ls_fc.jpg
│
├── 4-SkeletonCode/
│   ├── main.c
│   ├── encode.c
│   ├── encode.h
│   ├── decode.c
│   ├── decode.h
│   ├── common.h
│   ├── types.h
│   ├── beautiful.bmp
│   ├── secret.txt
│   └── Output.txt
│
├── 1-References/
│
├── 2-OutputImages/
│
└── README.md
```

## ⚙️ Features

- Encode secret text into BMP images.
- Decode hidden messages from stego images.
- Supports text file input.
- Uses LSB (Least Significant Bit) technique.
- Implements file handling in C.
- Uses command-line arguments.

## 🚀 How to Compile

Make sure GCC is installed on your system.

Open the terminal in the project directory and navigate to the source code folder:

```bash
cd 4-SkeletonCode
```

Compile the source files:

```bash
gcc main.c encode.c decode.c -o steganography
```

## ▶️ How to Run

### 1. Encoding

Use the following command to encode a secret message into a BMP image:

```bash
./steganography -e beautiful.bmp secret.txt stego.bmp
```

**Arguments:**
- `-e` : Encoding operation
- `beautiful.bmp` : Source BMP image
- `secret.txt` : Secret text file
- `stego.bmp` : Output image containing the hidden message

### 2. Decoding

Use the following command to decode the hidden message:

```bash
./steganography -d stego.bmp
```

**Arguments:**
- `-d` : Decoding operation
- `stego.bmp` : Image containing the hidden message

*Note: The exact command-line arguments depend on your implementation.*

## 🔍 Working Principle

1. **Input:** The user provides a BMP image and a secret text file.
2. **Encoding:** The program reads the image and secret message.
3. **Embedding:** Secret message bits are embedded into the least significant bits of image data.
4. **Output:** A new BMP image is generated with the hidden message.
5. **Decoding:** The program extracts the embedded bits from the stego image.
6. **Retrieval:** The original secret message is recovered and saved or displayed.

## 📚 Concepts Used

- C Programming
- File Handling
- Structures
- Pointers
- Bitwise Operators
- Command-Line Arguments
- Image Processing Basics
- LSB Steganography

## 💻 Sample Commands

### Compile

```bash
gcc main.c encode.c decode.c -o steganography
```

### Encode

```bash
./steganography -e beautiful.bmp secret.txt stego.bmp
```

### Decode

```bash
./steganography -d stego.bmp
```

## 📈 Applications

- Secure text communication
- Information hiding
- Digital image steganography
- Educational cybersecurity projects
- Understanding data embedding techniques


## 👨‍💻 Author

**N. Vinay Kumar**

🎓 B.Tech – Electronics and Communication Engineering (ECE)

🔗 **LinkedIn:** [N. Vinay Kumar](https://www.linkedin.com/in/n-vinay-kumar-846a48357/)

📧 **Email:** [vinaykumarvinay44072@gmail.com](mailto:vinaykumarvinay44072@gmail.com)

---

## 📜 License

This project is intended for educational and learning purposes.
