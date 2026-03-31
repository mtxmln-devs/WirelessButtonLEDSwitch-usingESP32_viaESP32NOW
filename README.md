# 📡  WirelessButtonLEDSwitch-usingESP32_viaESP32NOW

<div align="center"
  
Control an LED on one ESP32 board by pressing a button on another — wirelessly, with no router required.

[![Platform](https://img.shields.io/badge/Platform-ESP32-E7352C?style=flat&logo=espressif)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-00979D?style=flat&logo=arduino)](https://www.arduino.cc/)
[![Protocol](https://img.shields.io/badge/Protocol-ESP--NOW-informational?style=flat)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=c%2B%2B)](https://isocpp.org/)

</div>

---



## 📌 Overview

This project demonstrates **peer-to-peer wireless communication** between two ESP32 microcontrollers using **ESP-NOW** — Espressif's lightweight, connectionless protocol that operates without a Wi-Fi router or internet connection.

When a button is pressed on **Board 1**, it instantly sends a signal to **Board 2**, which turns on its LED. Releasing the button turns the LED off. Both boards run the **same sketch** — only the target MAC address changes.

---

## ✨ Features

- 📡 Wireless peer-to-peer communication — no router or internet required
- ⚡ Real-time response — near-instant LED reaction to button press
- 🔘 Momentary button support — press to turn ON, release to turn OFF
- 🛡️ 50ms debounce — prevents false triggers from button chatter
- 🔁 Two-way compatible — same code runs on both boards (just swap MAC address)
- 📟 Serial monitor feedback — logs PUSH: Sending ON and RELEASE: Sending OFF
- 🔒 No encryption overhead — lightweight, fast unencrypted data packets
- 💡 Simple data structure — single boolean (ledStatus) transmitted per packet

---


---

## 🛠️ Tech Stack

| Component | Detail |
|---|---|
| **Microcontroller** | ESP32 (×2) |
| **Protocol** | ESP-NOW (Espressif peer-to-peer) |
| **Language** | C++ (Arduino framework) |
| **IDE** | Arduino IDE |
| **Libraries** | `esp_now.h`, `WiFi.h` (built-in ESP32 core) |
| **Baud Rate** | 115200 |

---


## 🛠️ Hardware Required

| Component | Quantity |
|-----------|----------|
| ESP32 Development Board | 2 |
| Push Button (momentary) | 1 (on sender board) |
| LED | 1 (on receiver board) |
| 220Ω Resistor (for LED) | 1 |
| 10kΩ Resistor (optional, for button) | 1 |
| Jumper Wires | As needed |
| Breadboard | 1–2 |
| USB Cable | 2 |

> **Note:** The code uses `INPUT_PULLUP` for the button, so an external pull-up resistor is not required.

---

## 📐 Pin Configuration

| Pin | Function | Board |
|-----|----------|-------|
| GPIO 22 | Button Input (INPUT_PULLUP) | Board 1 (Sender) |
| GPIO 23 | LED Output | Board 2 (Receiver) |

> Both boards use the same pin numbers. The roles are determined by which MAC address is set as the target.


---


### Pin Reference

| Pin | Board | Component | Mode |
|---|---|---|---|
| GPIO 22 | Board 1 | Push Button | `INPUT_PULLUP` |
| GPIO 23 | Board 2 | LED | `OUTPUT` |
| GND | Both | Ground | — |

> 💡 `INPUT_PULLUP` means the pin reads **HIGH** when the button is not pressed and **LOW** when pressed. No external pull-up resistor needed.

---



### Wiring Diagram

**Button (Board 1):**
```
GPIO 22 ──── [ Button ] ──── GND
```
*(No external resistor needed — internal pull-up is enabled)*

**LED (Board 2):**
```
GPIO 23 ──── [220Ω Resistor] ──── LED (+) ──── GND
```

---

## 📋 MAC Addresses

Each ESP32 has a unique MAC address. You must know both boards' addresses before flashing.

| Board | MAC Address |
|-------|-------------|
| Board 1 | `B0:CB:D8:8A:69:E8` |
| Board 2 | `E0:8C:FE:30:A8:9C` |

### How to find your ESP32's MAC address

Upload and run this quick sketch on each board:
```cpp
#include <WiFi.h>
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
}
void loop() {}
```
Copy the output from the Serial Monitor.

---

## ⚙️ Software Setup

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (1.8.x or 2.x)
- ESP32 board package installed

### Installing the ESP32 Board Package

1. Open Arduino IDE → **File** → **Preferences**
2. Add this URL to *Additional Board Manager URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools** → **Board** → **Boards Manager**
4. Search for `esp32` and install the package by **Espressif Systems**

### Libraries Used

All libraries are **built-in** to the ESP32 Arduino core — no additional installs needed.

| Library | Purpose |
|---------|---------|
| `esp_now.h` | ESP-NOW peer-to-peer protocol |
| `WiFi.h` | Wi-Fi initialization (required by ESP-NOW) |

---

## 🚀 Flashing Instructions

### Step 1 — Configure the MAC address

In `12345.ino`, update `broadcastAddress` to point to the **other** board's MAC:

**For Board 1** (button sender → targets Board 2):
```cpp
uint8_t broadcastAddress[] = {0xE0, 0x8C, 0xFE, 0x30, 0xA8, 0x9C};
```

**For Board 2** (LED receiver → targets Board 1):
```cpp
uint8_t broadcastAddress[] = {0xB0, 0xCB, 0xD8, 0x8A, 0x69, 0xE8};
```

### Step 2 — Select board and port

- **Tools → Board** → `ESP32 Dev Module` (or your specific model)
- **Tools → Port** → Select the correct COM port

### Step 3 — Upload

Click **Upload** (→) in Arduino IDE. Repeat for the second board with its MAC address configured.

### Step 4 — Test

Open **Serial Monitor** at `115200` baud on Board 1. Press the button — you should see:
```
PUSH: Sending ON
RELEASE: Sending OFF
```
And the LED on Board 2 should light up accordingly.

---

## 🔍 How It Works

```
[ Button Pressed on Board 1 ]
        │
        ▼
  digitalRead(buttonPin) == LOW
        │
        ▼
  sendData.ledStatus = true
        │
        ▼
  esp_now_send() ──────────────────────► [ Board 2 receives data ]
                    (ESP-NOW, no router)          │
                                                  ▼
                                        OnDataRecv() callback fires
                                                  │
                                                  ▼
                                        digitalWrite(ledPin, HIGH)
                                                  │
                                                  ▼
                                           [ LED turns ON ]

[ Button Released on Board 1 ]
        │
        ▼
  sendData.ledStatus = false
        │
        ▼
  esp_now_send() ──────────────────────► [ Board 2 ]
                                                  │
                                                  ▼
                                        digitalWrite(ledPin, LOW)
                                                  │
                                                  ▼
                                           [ LED turns OFF ]
```

### Key Code Details

| Concept | Detail |
|---------|--------|
| Button logic | `INPUT_PULLUP` — pin reads `LOW` when pressed, `HIGH` when released |
| Debounce | 50ms `delay()` after any state change to prevent chatter |
| Data struct | `struct_message { bool ledStatus; }` — minimal 1-byte payload |
| Send trigger | Only sends on **state change**, not continuously |
| Receive callback | `OnDataRecv()` fires immediately when data arrives and drives the LED directly |
| Encryption | Disabled (`peerInfo.encrypt = false`) for simplicity |

---

## 📁 File Structure

```
📦 esp32-espnow-button-led/
├── 12345.ino       # Main Arduino sketch (flash to both boards)
└── README.md       # This file
```

---

## 🐛 Troubleshooting

| Problem | Likely Cause | Solution |
|---------|-------------|----------|
| LED doesn't respond | Wrong MAC address | Double-check `broadcastAddress` on both boards |
| `esp_now_init() failed` | Wi-Fi mode not set | Ensure `WiFi.mode(WIFI_STA)` runs before `esp_now_init()` |
| Button bouncing / flickering | Debounce too short | Increase `delay(50)` to `delay(100)` |
| No Serial output | Wrong baud rate | Set Serial Monitor to **115200** baud |
| Peer add failed | MAC address format error | Verify MAC bytes are in hex format `{0xXX, ...}` |
| Boards can't find each other | Different Wi-Fi channels | Set both boards to `peerInfo.channel = 0` (auto) |

---




















## 🔧 Setup & Installation

### Step 1 — Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software)

---

### Step 2 — Install ESP32 Board Support
1. Open Arduino IDE → **File → Preferences**
2. Add this URL to *Additional Board Manager URLs*:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
3. Go to **Tools → Board → Boards Manager**
4. Search **esp32** and install **esp32 by Espressif Systems**

---

### Step 3 — Find your MAC Addresses
Upload this sketch to **each board** separately to find their MAC addresses:

```cpp
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Serial.println(WiFi.macAddress());
}

void loop() {}
```

Open **Serial Monitor at 115200** and note the MAC address shown for each board.

---

### Step 4 — Configure the Sketch

Open `12345.ino` and update the `broadcastAddress` with the **OTHER board's MAC address**:

```cpp
// Board 1 MAC: B0:CB:D8:8A:69:E8
// Board 2 MAC: E0:8C:FE:30:A8:9C

// On Board 1 — paste Board 2's MAC here:
uint8_t broadcastAddress[] = {0xE0, 0x8C, 0xFE, 0x30, 0xA8, 0x9C};

// On Board 2 — paste Board 1's MAC here:
uint8_t broadcastAddress[] = {0xB0, 0xCB, 0xD8, 0x8A, 0x69, 0xE8};
```

---

### Step 5 — Upload to Both Boards

1. Select **Tools → Board → ESP32 Dev Module**
2. Select the correct **COM port** for each board
3. Click **Upload** (repeat for both boards)

---

### Step 6 — Test It

1. Open **Serial Monitor** at **115200 baud** on either board
2. Press the button on **Board 1**
3. You should see:
```
PUSH: Sending ON
RELEASE: Sending OFF
```
4. The **LED on Board 2** should light up while the button is held ✅

---

## 🛠️ Configuration

| Constant | Default | Description |
|---|---|---|
| `buttonPin` | `22` | GPIO pin for the push button |
| `ledPin` | `23` | GPIO pin for the LED |
| `broadcastAddress` | See code | MAC address of the OTHER board |
| `peerInfo.channel` | `0` | Wi-Fi channel (0 = auto) |
| `peerInfo.encrypt` | `false` | Encryption off for speed |
| Debounce delay | `50ms` | Prevents button chatter |

---

## ⚙️ How It Works

### Communication Flow

```
[ Board 1 ]                          [ Board 2 ]
    │                                     │
    │  Button PRESSED (pin → LOW)         │
    │  sendData.ledStatus = true          │
    │ ─────── ESP-NOW packet ──────────► │
    │                                     │  digitalWrite(ledPin, HIGH)
    │                                     │  💡 LED ON
    │                                     │
    │  Button RELEASED (pin → HIGH)       │
    │  sendData.ledStatus = false         │
    │ ─────── ESP-NOW packet ──────────► │
    │                                     │  digitalWrite(ledPin, LOW)
    │                                     │  💡 LED OFF
```

### Data Structure

Only **1 byte** is transmitted per packet — a single boolean:

```cpp
typedef struct struct_message {
    bool ledStatus;   // true = ON, false = OFF
} struct_message;
```

### Debounce Logic

```
Button physical state changes
         │
         ▼
Is it different from last known state?
         │
    YES  ▼
Send ESP-NOW packet → wait 50ms → update lastPhysicalState
         │
    NO   ▼
Do nothing (ignore noise/chatter)
```

---

## 📟 Serial Monitor Output

```
PUSH: Sending ON
RELEASE: Sending OFF
PUSH: Sending ON
PUSH: Sending ON
RELEASE: Sending OFF
```

---

## 📁 Project Structure

```
ESP32-ESPNOW-Button-LED/
├── 12345.ino       # Main Arduino sketch (upload to both boards)
└── README.md       # Project documentation
```

> 💡 The **same `.ino` file** is used on both boards — only the `broadcastAddress` value changes.

---

## 🔧 Troubleshooting

| Problem | Fix |
|---|---|
| LED not responding | Double-check the `broadcastAddress` matches the other board's MAC exactly |
| `esp_now_init()` fails | Make sure `WiFi.mode(WIFI_STA)` is called before `esp_now_init()` |
| Upload fails | Select correct board (**ESP32 Dev Module**) and COM port in Arduino IDE |
| Serial Monitor shows nothing | Set baud rate to **115200** in Serial Monitor |
| Button triggers multiple times | Increase debounce delay from `50` to `100` ms |
| LED stays ON / stays OFF | Verify LED wiring — long leg (+) to GPIO 23, short leg (−) to GND via resistor |




## 🙌 References

- [Espressif ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [ESP-NOW Overview — Random Nerd Tutorials](https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/)

---

>







