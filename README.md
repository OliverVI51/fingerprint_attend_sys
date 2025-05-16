# 📌 Arduino-Based Fingerprint Attendance System

This project is an **automated attendance system** using an **Arduino microcontroller** and a **fingerprint sensor** to identify individuals and log attendance securely and efficiently. Designed for use in schools, offices, or restricted areas, it eliminates the need for manual attendance tracking and prevents proxy attendance.

---

## 🔧 Features

- ✅ Fingerprint registration and verification  
- 🕒 Real-time attendance logging  
- 🔊 Voice feedback using DFPlayer Mini and speaker  
- 💾 Stores attendance data to EEPROM/SD card (based on configuration)  
- 📟 LCD/serial monitor display for status and feedback  
- 🔔 Optional buzzer or LED indicators for user interaction  
- 🕹️ Simple push-button interface  

---

## 🛠️ Tech Stack

- **Hardware:**  
  - Arduino Uno/Nano  
  - R305/GT511C3 Fingerprint Sensor  
  - 16x2 LCD (I2C)  
  - DFPlayer Mini MP3 Module  
  - Speaker  
  - RTC Module *(optional)*  
  - Buzzer
  - LED

- **Software:**  
  - Arduino IDE  
  - C/C++

---

## 📁 Project Structure

├── /src # Main codebase and Arduino sketches
├── /docs # Circuit diagrams and setup instructions
├── /images # Hardware setup photos
└── README.md # Project description


---

## 🚀 Getting Started

1. Clone this repository
2. Open the main `.ino` file in Arduino IDE
3. Upload the code to your Arduino board
4. Use the serial monitor or LCD interface to enroll and verify fingerprints
5. Attendance logs will be shown or stored (based on your setup)

---

## 📷 Preview

> *(Insert images or GIFs of your system in action here)*

---

## 🔒 Security Considerations

- Fingerprints are stored with unique IDs
- Prevents duplicate or unauthorized access
- Hardware-level validation ensures accuracy

---

## 📃 License

This project is licensed under the **GNU General Public License v3.0**.  
You may redistribute and/or modify it under the terms of the GPL-3.0.  
See the [LICENSE](LICENSE) file for more details.  
🔗 [Learn more about GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)

---

## 🙌 Acknowledgements

Special thanks to the Arduino and open hardware communities for documentation, sensor libraries, and inspiration.
