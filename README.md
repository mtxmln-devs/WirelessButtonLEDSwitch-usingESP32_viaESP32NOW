# 📡 WirelessButtonLEDSwitch — ESP32 via ESP-NOW

<div align="center">

[![Platform](https://img.shields.io/badge/Platform-ESP32-E7352C?style=flat&logo=espressif)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-00979D?style=flat&logo=arduino)](https://www.arduino.cc/)
[![Protocol](https://img.shields.io/badge/Protocol-ESP--NOW-informational?style=flat)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=c%2B%2B)](https://isocpp.org/)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat)

Control an LED on one ESP32 board by pressing a button on another —  
**wirelessly, with no router required.** 🚀

[📌 Overview](#-overview) · [🛠️ Hardware](#️-hardware-required) · [⚙️ Setup](#️-software-setup) · [🚀 Flashing](#-flashing-instructions) · [🔍 How It Works](#-how-it-works)

</div>

---

## 📌 Overview

This project demonstrates **peer-to-peer wireless communication** between two ESP32 microcontrollers using **ESP-NOW** — Espressif's lightweight, connectionless protocol that works without a Wi-Fi router or internet connection.

When a button is pressed on **Board 1**, it instantly sends a signal to **Board 2**, which turns on its LED. Releasing the button turns the LED off. Both boards run the **same sketch** — only the target MAC address changes.

---

## ✨ Features

- 📡 **Wireless peer-to-peer** communication — no router or internet required
- ⚡ **Real-time response** — near-instant LED reaction to button press
- 🔘 **Momentary button support** — press to turn ON, release to turn OFF
- 🛡️ **50ms debounce** — prevents false triggers from button chatter
- 🔁 **Two-way compatible** — same code runs on both boards (just swap MAC address)
- 📟 **Serial monitor feedback** — logs `PUSH: Sending ON` and `RELEASE: Sending OFF`
- 🔒 **No encryption overhead** — lightweight, fast unencrypted data packets
- 💡 **Simple data structure** — single boolean (`ledStatus`) transmitted per packet

---

## 🛠️ Tech Stack

| Component | Detail |
|---|---|
| **Microcontroller** | ESP32 (×2) |
| **Protocol** | ESP-NOW (Espressif peer-to-peer) |
| **Language** | C++ (Arduino framework) |
| **IDE** | Arduino IDE |
| **Libraries** | `esp_now.h`, `WiFi.h` (built-in to ESP32 core) |
| **Baud Rate** | 115200 |

---

## 🛠️ Hardware Required

| Component | Quantity |
|---|---|
| ESP32 Development Board | 2 |
| Push Button (momentary) | 1 — on Board 1 (sender) |
| LED | 1 — on Board 2 (receiver) |
| 220Ω Resistor (for LED) | 1 |
| 10kΩ Resistor (optional, for button) | 1 |
| Jumper Wires | As needed |
| Breadboard | 1–2 |
| USB Cable | 2 |

> 💡 The code uses `INPUT_PULLUP` for the button — no external pull-up resistor needed.

---

## 📐 Wiring Diagram

**Board 1 — Sender (button):**
```
GPIO 22 ──── [ Button ] ──── GND
```

**Board 2 — Receiver (LED):**
```
GPIO 23 ──── [ 220Ω Resistor ] ──── LED (+) ──── GND
```

### Pin Reference

| Pin | Board | Component | Mode |
|---|---|---|---|
| GPIO 22 | Board 1 | Push Button | `INPUT_PULLUP` |
| GPIO 23 | Board 2 | LED | `OUTPUT` |
| GND | Both | Ground | — |

> ⚠️ `INPUT_PULLUP` means the pin reads **HIGH** when the button is not pressed and **LOW** when pressed.

---

## 📋 MAC Addresses

Each ESP32 has a unique MAC address. You must know both before flashing.

| Board | MAC Address |
|---|---|
| Board 1 | `B0:CB:D8:8A:69:E8` |
| Board 2 | `E0:8C:FE:30:A8:9C` |

### How to find your MAC address

Upload this quick sketch to each board and open Serial Monitor at `115200`:

```cpp
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Serial.println(WiFi.macAddress());
}

void loop() {}
```

---

## ⚙️ Software Setup

### Step 1 — Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software) (v1.8.x or v2.x)

### Step 2 — Install ESP32 Board Package

1. Open Arduino IDE → **File → Preferences**
2. Add this URL to *Additional Board Manager URLs*:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
3. Go to **Tools → Board → Boards Manager**
4. Search `esp32` and install **esp32 by Espressif Systems**

### Libraries Used

All libraries are **built-in** to the ESP32 Arduino core — no additional installs needed.

| Library | Purpose |
|---|---|
| `esp_now.h` | ESP-NOW peer-to-peer protocol |
| `WiFi.h` | Wi-Fi initialization (required by ESP-NOW) |

---

## 🚀 Flashing Instructions

### Step 1 — Configure the MAC Address

In `12345.ino`, update `broadcastAddress` to point to the **other** board's MAC:

**On Board 1** (sends to Board 2):
```cpp
uint8_t broadcastAddress[] = {0xE0, 0x8C, 0xFE, 0x30, 0xA8, 0x9C};
```

**On Board 2** (sends to Board 1):
```cpp
uint8_t broadcastAddress[] = {0xB0, 0xCB, 0xD8, 0x8A, 0x69, 0xE8};
```

### Step 2 — Select Board and Port

- **Tools → Board** → `ESP32 Dev Module`
- **Tools → Port** → Select the correct COM port

### Step 3 — Upload

Click **Upload (→)** in Arduino IDE. Repeat for the second board with its MAC address configured.

### Step 4 — Test

Open **Serial Monitor at 115200 baud** on Board 1. Press the button — you should see:

```
PUSH: Sending ON
RELEASE: Sending OFF
```

And the **LED on Board 2** should light up accordingly ✅

---

## 🔍 How It Works

### Communication Flow

```
[ Board 1 ]                               [ Board 2 ]
     │                                         │
     │  Button PRESSED (pin → LOW)             │
     │  sendData.ledStatus = true              │
     │ ──────── ESP-NOW packet ─────────────► │
     │                                         │  OnDataRecv() fires
     │                                         │  digitalWrite(ledPin, HIGH)
     │                                         │  💡 LED ON
     │                                         │
     │  Button RELEASED (pin → HIGH)           │
     │  sendData.ledStatus = false             │
     │ ──────── ESP-NOW packet ─────────────► │
     │                                         │  digitalWrite(ledPin, LOW)
     │                                         │  💡 LED OFF
```

### Data Structure

Only **1 byte** is transmitted per packet — a single boolean:

```cpp
typedef struct struct_message {
    bool ledStatus;   // true = LED ON,  false = LED OFF
} struct_message;
```

### Debounce Logic

```
Button physical state changes
          │
          ▼
  Different from last known state?
          │
    YES   ▼
  Send ESP-NOW packet
  → wait 50ms
  → update lastPhysicalState
          │
    NO    ▼
  Do nothing (ignore noise / chatter)
```

### Key Code Details

| Concept | Detail |
|---|---|
| Button logic | `INPUT_PULLUP` — reads `LOW` when pressed, `HIGH` when released |
| Debounce | 50ms `delay()` after any state change to prevent chatter |
| Data struct | `struct_message { bool ledStatus; }` — minimal 1-byte payload |
| Send trigger | Only sends on **state change**, not continuously |
| Receive callback | `OnDataRecv()` fires immediately and drives the LED directly |
| Encryption | Disabled (`peerInfo.encrypt = false`) for simplicity and speed |

---

## 🛠️ Configuration Reference

| Constant | Default | Description |
|---|---|---|
| `buttonPin` | `22` | GPIO pin for the push button |
| `ledPin` | `23` | GPIO pin for the LED |
| `broadcastAddress` | See code | MAC address of the OTHER board |
| `peerInfo.channel` | `0` | Wi-Fi channel (0 = auto) |
| `peerInfo.encrypt` | `false` | Encryption disabled for speed |
| Debounce delay | `50ms` | Prevents button chatter |

---

## 📟 Serial Monitor Output

```
PUSH: Sending ON
RELEASE: Sending OFF
PUSH: Sending ON
RELEASE: Sending OFF
```

---

## 📁 Project Structure

```
WirelessButtonLEDSwitch-usingESP32_viaESP32NOW/
├── 12345.ino       # Main Arduino sketch — flash to both boards
└── README.md       # Project documentation
```

> 💡 The **same `.ino` file** is uploaded to both boards — only the `broadcastAddress` value differs.

---

## 🐛 Troubleshooting

| Problem | Likely Cause | Fix |
|---|---|---|
| LED doesn't respond | Wrong MAC address | Double-check `broadcastAddress` on both boards |
| `esp_now_init()` fails | Wi-Fi not initialized | Ensure `WiFi.mode(WIFI_STA)` runs before `esp_now_init()` |
| Button triggers multiple times | Debounce too short | Increase `delay(50)` to `delay(100)` |
| No Serial output | Wrong baud rate | Set Serial Monitor to **115200** baud |
| Upload fails | Wrong board/port selected | Select **ESP32 Dev Module** and correct COM port |
| Peer add failed | MAC format error | Verify MAC bytes are in hex format `{0xXX, ...}` |
| Boards can't find each other | Channel mismatch | Set both boards to `peerInfo.channel = 0` (auto) |
| LED stays ON / stays OFF | Wiring issue | Long leg (+) to GPIO 23, short leg (−) to GND via 220Ω |

---

## 🚀 Possible Extensions

- [ ] Add **bidirectional control** — button on each board controls the other's LED
- [ ] Support **multiple receivers** — one sender broadcasting to many boards
- [ ] Send **sensor data** — temperature, distance, or humidity instead of button state
- [ ] Enable **encrypted communication** — set `peerInfo.encrypt = true`
- [ ] Use **deep sleep** between transmissions to save battery power
- [ ] Add an **OLED display** to show connection status and received values

---

## 🙌 References

- [Espressif ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [ESP-NOW Guide — Random Nerd Tutorials](https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/)


