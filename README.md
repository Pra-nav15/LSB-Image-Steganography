# 🖼️ Image Steganography using LSB in C

## 📌 Overview
The **LSB Image Steganography System** is a command-line application developed in the **C programming language** that hides secret information inside digital images using the **Least Significant Bit (LSB) technique**. Steganography is the art of concealing the existence of communication by embedding secret data within another file so that the hidden information is not noticeable.

This project allows users to **hide confidential text data inside a `.bmp` image file** and later retrieve it safely. The application works by modifying the **least significant bits of image pixels**, which does not noticeably change the image quality. Since the human eye cannot detect these small changes, the hidden message remains invisible to normal observation.

The system operates through **two main modes**:
- **Encoding** – hiding secret data inside an image.
- **Decoding** – extracting the hidden data from the image.

---

# ✨ Features

## 🔐 Encoding (Hide Data)
During encoding, the application:
- Accepts a **BMP image file** and a **text file containing the secret message**.
- Validates all input files.
- Analyzes the **size of the message file** to ensure the image has enough capacity to store the data.
- Embeds the following information inside the image:
  - Magic string
  - File extension
  - File size
  - Secret message data
- Stores the encoded output as a new **steganographic image**.

## 🔍 Decoding (Extract Data)
During decoding, the application:
- Reads the encoded BMP image.
- Detects the **magic string** to confirm the image contains hidden data.
- Extracts the hidden message accurately.
- Reconstructs the original secret file.

---

# 🧠 What is Steganography?

Steganography is the practice of **hiding private or sensitive information within another file** so that the existence of the information is concealed. Unlike cryptography, which encrypts data but makes it obvious that secret data exists, steganography hides the data in such a way that **no one suspects that hidden communication is taking place**.

Digital images are commonly used as carrier files because they are widely shared across the internet and contain large amounts of data that can store hidden information without visible distortion.

Steganography works by exploiting **human perception**. Human vision cannot easily detect very small changes in image pixel values. By modifying the **least significant bits of pixel data**, secret information can be embedded without visibly altering the image.

---

# ⚙️ Why BMP Images?
This project supports **only `.bmp` images** because:
- BMP images are **uncompressed**
- They allow **direct byte-level manipulation**
- Pixel data can be modified safely without affecting image integrity

---

# 📋 Requirements

The application performs the following operations:

- Accepts an **image file (.bmp)** and a **text file containing the secret message**.
- Analyzes the **size of the message** to verify that it can fit within the image.
- Provides an option to embed a **magic string** to identify whether the image contains hidden data.
- Allows **decoding (decryption)** to retrieve the hidden information.
- Operates entirely through **command-line arguments**.

---

# ⚠️ Error Handling

The system includes robust validation and error handling to detect:

- Invalid image files
- Missing command-line arguments
- Unsupported file formats
- Insufficient image capacity for storing the secret message

Clear error messages guide the user when incorrect input is provided.

---

# 🛠 Technologies Used

This project demonstrates the practical application of several **C programming concepts**, including:

- 📂 File Handling
- 📍 Pointers
- 🧩 Structures
- ⚙ Bitwise Operations
- 🧾 Command-Line Arguments
- 📜 Standard Input/Output operations

---

# 🎯 Applications

Steganography has many practical applications, including:

- 🔐 Secure data transmission
- 🪖 Defense communication
- 💧 Digital watermarking
- 📁 Confidential information sharing
- 🛡 Protecting sensitive data

---

# ⭐ Advantages

- Maintains **data confidentiality**
- Simple and reliable implementation
- Preserves **original image quality**
- Hidden information is **invisible to normal users**

---

# 🎓 Learning Outcomes

By implementing this project, the following skills are developed:

- Understanding **image data manipulation**
- Working with **bit-level operations**
- Implementing **secure data hiding techniques**
- Managing **file processing in C**
- Building **command-line applications**

---

# 👨‍💻 Author
**Pranav Prasad**
