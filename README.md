# ☀️ Solar Powered Smart Grid with Load Prioritization

An Arduino-based smart energy management system that monitors battery voltage from a solar-charged battery and intelligently controls loads based on three priority levels — automatically shedding low-priority loads as battery level drops, and falling back to the grid during critical conditions.

---

## 📌 Table of Contents

- [System Overview](#-system-overview)
- [Block Diagram Logic](#-block-diagram-logic)
- [Load Priority Levels](#-load-priority-levels)
- [Voltage Thresholds & Load Control Logic](#-voltage-thresholds--load-control-logic)
- [Hardware Requirements](#-hardware-requirements)
- [Pin Configuration](#-pin-configuration)
- [Voltage Divider Circuit](#-voltage-divider-circuit)
- [LCD Display](#-lcd-display)
- [Serial Monitor Output](#-serial-monitor-output)
- [Getting Started](#-getting-started)
- [Project Structure](#-project-structure)
- [Future Improvements](#-future-improvements)

---

## 🔍 System Overview

This project implements a **Solar-Powered Smart Grid** that:

- Reads battery voltage through a resistor voltage divider on analog pin `A0`
- Monitors grid availability via a digital sensor on pin `D2`
- Controls three relay channels based on real-time battery state
- Displays live voltage and load status on a **16×2 LCD**
- Logs state changes to the **Serial Monitor**

The system prioritizes critical loads (emergency lights, medical equipment) and progressively cuts off medium and low-priority loads as battery charge decreases.

---

## 🗂️ Block Diagram Logic

```
Solar Panel
    │
    ▼
Solar Charge Controller ──► Battery ──► Voltage Divider ──► Arduino (A0)
                                  │
                         Grid Sensor ──────────────────────► Arduino (D2)
                                  │
                             Arduino
                                  │
                          Relay Module (3 Channels)
                         /          |           \
                        D10        D11          D13
                         │          │             │
               High Priority   Medium Priority  Low Priority
                  Loads           Loads           Loads
         (Emergency Lights,  (Fans, Computers,  (AC, Washing
          Medical Equipment)   Water Pump)    Machine, EV Charger)
```

---

## ⚡ Load Priority Levels

| Priority | Relay Pin | Loads | Behavior |
|----------|-----------|-------|----------|
| 🔴 **High** | `D10` | Emergency Lights, Medical Equipment | **Always ON** (cut only at CRITICAL) |
| 🟡 **Medium** | `D11` | Fans & Computers, Water Pump | OFF below 11.0V |
| 🟢 **Low** | `D13` | Air Conditioners, Washing Machine, EV Charger | OFF below 12.0V |

---

## 📊 Voltage Thresholds & Load Control Logic

| Battery Voltage | System State | High Priority | Medium Priority | Low Priority |
|-----------------|-------------|:---:|:---:|:---:|
| ≥ **12.0V** | Full | ✅ ON | ✅ ON | ✅ ON |
| ≥ **11.0V** | Low | ✅ ON | ✅ ON | ❌ OFF |
| ≥ **10.8V** | Medium-Low | ✅ ON | ❌ OFF | ❌ OFF |
| < **10.8V** | ⚠️ CRITICAL | ❌ OFF | ❌ OFF | ❌ OFF → Grid |

> **At CRITICAL voltage**, all relay outputs are turned OFF and the system signals a **grid fallback**. The LCD alternates between `"CRITICAL-> GRID"` and `"Medium & Low OFF"` messages.

---

## 🔧 Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | Arduino Uno / Nano |
| Display | 16×2 LCD (HD44780, parallel interface) |
| Relay Module | 3-channel, 5V active-HIGH |
| Voltage Divider | R1 = 10kΩ, R2 = 2.2kΩ |
| Battery | 12V Lead-Acid / Li-Ion |
| Solar Charge Controller | Any PWM/MPPT compatible with 12V battery |
| Grid Sensor | Optocoupler / voltage presence detector |

---

## 🔌 Pin Configuration

| Arduino Pin | Direction | Function |
|-------------|-----------|----------|
| `A0` | INPUT | Battery voltage (via voltage divider) |
| `D2` | INPUT | Grid availability sensor |
| `D4–D9` | OUTPUT | LCD data & control pins |
| `D10` | OUTPUT | Relay — High Priority Loads |
| `D11` | OUTPUT | Relay — Medium Priority Loads |
| `D13` | OUTPUT | Relay — Low Priority Loads |

---

## ⚙️ Voltage Divider Circuit

Battery voltage (up to ~14V) is stepped down to the Arduino's safe 0–5V ADC range using a resistor divider:

```
Battery (+) ──── R1 (10kΩ) ──┬──── R2 (2.2kΩ) ──── GND
                              │
                             A0 (Arduino)
```

**Conversion formula used in firmware:**

```cpp
float adcVoltage     = adcValue * (5.0 / 1023.0);
float batteryVoltage = adcVoltage * 5.545;  // Calibrated scaling factor
```

> The scaling factor `5.545` is derived from `(R1 + R2) / R2 = 12200 / 2200 ≈ 5.545` and may require fine-tuning based on actual resistor tolerances.

---

## 🖥️ LCD Display

The 16×2 LCD shows live system status:

```
┌────────────────┐
│ Vbat: 12.34V   │  ← Row 0: Real-time battery voltage
│ ALL LOADS ARE ON│  ← Row 1: Current load state
└────────────────┘
```

**Row 1 messages:**

| Message | Condition |
|---------|-----------|
| `ALL LOADS ARE ON` | Vbat ≥ 12.0V |
| `LOW LOAD OFF` | 11.0V ≤ Vbat < 12.0V |
| `Medium & Low OFF` | 10.8V ≤ Vbat < 11.0V |
| `CRITICAL-> GRID` ↔ `Medium & Low OFF` | Vbat < 10.8V (alternating every 1–2s) |

---

## 🖨️ Serial Monitor Output

Serial output is logged at **9600 baud** and only printed when voltage changes by more than **0.05V** (to avoid flooding):

```
Battery Voltage : 12.43 V
All Loads are ON
Battery Voltage : 11.87 V
Low Priority OFF
Battery Voltage : 10.75 V
Medium & Low OFF
CRITICAL : Switched to Grid
```

---

## 🚀 Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+ or v2.x)
- `LiquidCrystal` library (built-in with Arduino IDE)

### Steps

1. **Clone this repository**
   ```bash
   git clone https://github.com/Er-Nikhil-Sutar/Solar-Powered-Smart-Grid-with-Load-Prioritization.git
   cd solar-smart-grid-arduino
   ```

2. **Wire the hardware** as per the pin configuration table above

3. **Open** `solar_smart_grid.ino` in Arduino IDE

4. **Calibrate** the voltage scaling factor if needed:
   - Measure actual battery voltage with a multimeter
   - Adjust `5.545` in `readBatteryVoltage()` to match

5. **Upload** to your Arduino board

6. **Open Serial Monitor** at `9600 baud` to observe logs

---

## 📁 Project Structure

```
solar-smart-grid-arduino/
├── solar_smart_grid.ino   # Main firmware file
├── README.md              # Project documentation
```

---

## 🔮 Future Improvements

- [ ] Add solar panel voltage/current monitoring (INA219)
- [ ] EEPROM logging of min/max voltage history
- [ ] GSM/WiFi alerts when battery reaches CRITICAL state
- [ ] MPPT charge controller integration via UART
- [ ] Add a buzzer alarm for critical battery events
- [ ] Replace polling `delay()` with full non-blocking `millis()` logic

---

## 📄 License

This project is open-source under the [MIT License](LICENSE).

---

## 🙋 Author

**Nikhil** — Embedded Systems Developer  
