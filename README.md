# ESP32 Programming & Simulation Guide (PlatformIO + VS Code + Wokwi)

[🇻🇳 Tiếng Việt](README_VI.md) | [🇬🇧 English](README.md)

A comprehensive guide and starter template for **setting up, programming, and simulating ESP32 circuits** directly within **Visual Studio Code** using **PlatformIO** and the **Wokwi Simulator** extension.

This repository helps developers, students, and makers learn, practice, and prototype IoT / ESP32 applications entirely in a virtual environment (no physical hardware required) along with a rich collection of modular peripheral examples.

---

## 🎯 Purpose of This Repository

1. **ESP32 Development on VS Code**: Experience a professional workflow with PlatformIO as an alternative to the traditional Arduino IDE.
2. **Fast Virtual Prototyping with Wokwi**: Test code logic, verify peripheral wiring, and debug circuits safely without the risk of hardware damage.
3. **Simulate Real Internet Connectivity (Wi-Fi & MQTT)**: Transmit and receive real-time IoT data between the virtual microcontroller and Cloud/MQTT brokers.
4. **Rich Reference Examples (Examples)**: Ready-to-use code snippets and circuit configurations for various popular sensors and peripherals.

---

## 🛠️ 1. Prerequisites & Environment Setup (One-time Setup)

You only need a computer with the following software installed:

1. **[Visual Studio Code](https://code.visualstudio.com/)**.
2. **PlatformIO IDE Extension**:
   - Open the **Extensions** view in VS Code (`Ctrl + Shift + X`).
   - Search for `PlatformIO IDE` and click **Install**.
3. **Wokwi Simulator Extension**:
   - In the **Extensions** view, search for `Wokwi Simulator` and click **Install**.
   - *(On first use, Wokwi may prompt you to sign in with a free account to activate your simulation license).*

---

## 🚀 2. Simulation Workflow in VS Code

All necessary dependencies, board configurations, and library declarations are pre-configured in [`platformio.ini`](platformio.ini). Simply follow these 3 steps:

### Step 1: Open the Project
- Open VS Code > select **File > Open Folder...** > select the `ESP32` directory.
- Wait a few moments for PlatformIO to initialize the environment and download dependencies automatically.

### Step 2: Build the Firmware
- Press **`Ctrl + Alt + B`** or run the following command in the VS Code terminal:
  ```bash
  pio run
  ```
  *(PlatformIO will compile the source code and generate the binary at `.pio/build/esp32doit-devkit-v1/firmware.bin`).*

### Step 3: Launch the Wokwi Simulation
- Open the circuit diagram file [`diagram.json`](diagram.json) in VS Code.
- Click the green **Play (Start Simulation)** button in the top-right corner, or press `F1` > type `Wokwi: Start Simulator`.
- **Interact directly with the simulation:**
  - Click on the **DHT22** sensor to adjust the temperature and humidity sliders.
  - View real-time measurements displayed on the **OLED** screen.
  - Hear the **Buzzer** tone and watch the **Relay / LED** toggle when temperature thresholds are exceeded.
  - Monitor diagnostic logs in the **Wokwi Serial Monitor** window.

---

## 📋 3. Main Demo Program (`src/main.c++`)

The default program in [`src/main.c++`](src/main.c++) demonstrates an end-to-end IoT environmental monitoring and alert system:

- **Sensor Reading**: Continuously reads temperature and humidity from **DHT22** (GPIO 25).
- **OLED Display (SSD1306 I2C)**: Displays Wi-Fi status, MQTT status, and live sensor readings in real time (SDA: GPIO 21, SCL: GPIO 22).
- **Smart Alert Logic**: Automatically triggers the **Buzzer** (GPIO 27, 2000 Hz) and activates the **Relay / Warning LED** (GPIO 5) if temperature exceeds safe thresholds (`> 35°C` or `< 18°C`).
- **Cloud MQTT Communication**:
  - **Publish**: Publishes a JSON payload every 2 seconds: `{"temperature": 28.5, "humidity": 70.0}` to topic `conghoa/esp32`.
  - **Subscribe**: Listens for remote control commands to toggle the relay/warning device via `on` / `off` text or `{"led":"on"}` / `{"led":"off"}` JSON.
- **FreeRTOS Multi-tasking**: Separates sensor reading, OLED updates, and MQTT publishing into an independent task (`SendData`) running on **Core 0**, preventing blocking issues in the main `loop()`.

### Pinout Mapping:
```text
           ESP32 DevKit V1
         +-----------------+
         |           3V3/5V| ---> VCC Power (DHT22, OLED, Buzzer, Relay)
         |              GND| ---> Common GND
(DATA)   | GPIO 25         | <--- DHT22 (Sensor Signal Out)
(SDA)    | GPIO 21         | <--- OLED Display (SDA)
(SCL)    | GPIO 22         | <--- OLED Display (SCL)
(Signal) | GPIO 27         | ---> Buzzer (Positive Pin)
(IN)     | GPIO 5          | ---> Relay Module / Warning LED
         +-----------------+
```

---

## 📚 4. Reference Examples Library (`examples/`)

The [`examples/`](examples) folder contains modular, self-contained examples for learning individual peripherals:

| Example Folder | Peripheral / Feature | Description |
| :--- | :--- | :--- |
| [`examples/buzzer/`](examples/buzzer) | **Buzzer** | Play tones, beeps, and audio melodies via GPIO |
| [`examples/dht/`](examples/dht) | **DHT22 Sensor** | Read temperature and humidity, printed to Serial |
| [`examples/lcd/`](examples/lcd) | **LCD I2C 20x4** | Display characters and formatted data on an I2C LCD |
| [`examples/oled/`](examples/oled) | **OLED SSD1306** | Text formatting, layout alignment, and graphics rendering |
| [`examples/relay/`](examples/relay) | **Relay Module** | Control AC/DC loads via push button or automatic modes |
| [`examples/rtc/`](examples/rtc) | **RTC DS1307** | Read Date, Month, Year, Hour, Minute, and Second |
| [`examples/servo/`](examples/servo) | **Servo Motor** | Control servo rotation angles from $0^\circ$ to $180^\circ$ |
| [`examples/mqtt/`](examples/mqtt) | **MQTT + LCD** | Connect to MQTT broker and display incoming messages on LCD |
| [`examples/mqtt+oled/`](examples/mqtt+oled) | **MQTT + OLED** | MQTT connectivity combined with OLED display and LED control |
| [`examples/web/`](examples/web) | **Web Dashboard** | Web-based HTML/JS interface controlling MQTT over WebSockets |

### 💡 How to test any example:
1. Open the source code file inside the desired example folder (e.g., `examples/servo/main.cpp` or `examples/lcd/main.cpp`).
2. Copy the entire contents and paste into [`src/main.c++`](src/main.c++).
3. Press **`Ctrl + Alt + B`** to recompile.
4. Open the corresponding `diagram.json` (if included in that example) or update the root [`diagram.json`](diagram.json) to start simulation.

---

## 🌐 5. Testing MQTT Communication from External Tools

Even when running in Wokwi, the simulated ESP32 connects to the real Internet using the virtual `Wokwi-GUEST` network and exchanges real MQTT messages with online brokers!

You can use **[MQTTX](https://mqttx.app/)**, web clients, or command-line tools to interact:

1. **Client Connection Settings**:
   - Host / Broker: `44.232.241.40` (or `broker.emqx.io`, `broker.hivemq.com` as configured)
   - Port: `1883`
2. **Receive Sensor Data from Simulated ESP32**:
   - Subscribe to topic: `conghoa/esp32`
   - You will see live telemetry JSON payloads `{ "temperature": ..., "humidity": ... }` arriving every 2 seconds.
3. **Send Control Commands to Simulated ESP32**:
   - Publish to topic `conghoa/esp32` with payload `{"led":"on"}` or `{"led":"off"}`.
   - Watch in Wokwi: The Relay and Warning LED will instantly toggle state.

---

## 🔌 6. Flashing to Physical ESP32 Hardware (Optional)

If you have physical hardware and wish to deploy the code:

1. Open [`src/main.c++`](src/main.c++), change the Wi-Fi credentials from `Wokwi-GUEST` to your local Wi-Fi SSID and password:
   ```cpp
   WiFi.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
   ```
2. Connect your ESP32 board to your computer using a USB data cable.
3. Click the **Upload** button (`→` icon in the bottom status bar) or run:
   ```bash
   pio run --target upload
   ```
4. Open the Serial Monitor at 115200 baud:
   ```bash
   pio device monitor --baud 115200
   ```

---

## ❓ 7. Troubleshooting & Tips

- **Wokwi error: firmware.bin not found**:
  Make sure you have compiled the code at least once using `Ctrl + Alt + B` (or `pio run`) before starting the simulator.
- **Wi-Fi not connecting in Wokwi**:
  Inside the Wokwi simulation environment, always keep the SSID as `"Wokwi-GUEST"` and the password empty `""`.
- **Serial Monitor shows corrupted characters**:
  Ensure the baud rate matches `115200` (pre-set in `platformio.ini` via `monitor_speed = 115200`).
- **Adding new external libraries**:
  Simply open [`platformio.ini`](platformio.ini) and append the library name under `lib_deps`. PlatformIO will automatically fetch it during the next build.
