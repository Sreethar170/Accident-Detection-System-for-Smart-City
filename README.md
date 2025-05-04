# 🚗 Accident Detection System for Smart City

A **Zone-Level Real-Time Accident Detection System** designed to improve emergency response efficiency and road safety. This IoT-based solution integrates **GPS**, **mass-type sensors**, and **networked communication towers** (similar to Jio/BSNL) to detect vehicle accidents, provide driver assistance, and transmit critical alerts to nearby zone headquarters.

---

## 🔍 Project Overview

This system is designed for use in smart cities where vehicle safety, real-time accident reporting, and faster emergency services are crucial.

### ✅ Key Features

- **Accident Detection:** Uses a mass-type sensor to detect collision impact.
- **Smart Delay Mechanism:** 
  - A 1-minute timer starts after an accident.
  - Voice alert prompts the driver.
  - If the driver is conscious, they can cancel the alert using a stop button.
- **Automatic Alert System:**
  - If not cancelled, alert is sent via network (tower-based or GPS).
  - Zone HQ receives detailed accident data.
- **Fallback Mechanism:** If GPS fails, the system uses tower ID (Cell ID) for location identification.

---

## 🧠 Technologies Used

- **Microcontroller:** Arduino Nano  
- **Wireless Communication:** nRF24L01 Module  
- **Location Tracking:** GPS Module  
- **Sensors:** Mass-Type Collision Sensor  
- **Voice Feedback:** Voice alert system (buzzer + audio module)  
- **Button Inputs:** Stop button & condition check logic  
- **Networking:** Simulated tower network (like Jio/BSNL)

---

## 📦 System Architecture

1. **Vehicle Unit:**
   - Mass sensor detects impact.
   - Starts timer and gives voice alert.
   - Sends data if not cancelled.

2. **Zone Tower:**
   - Receives data from vehicle.
   - Forwards alert to zone headquarters.

3. **Zone Headquarters:**
   - Displays vehicle and driver details.
   - Locates accident via GPS or tower ID.

---

## 🧾 Alert Message Details

Sent message includes:

- Vehicle Owner Name  
- Driver Name  
- Driver Age & Blood Group  
- Contact Number  
- Vehicle Info  
- Home Address  
- Current Location (GPS or Cell ID)  

---

## 📸 Demo & Images

> _(Add images/videos/screenshots of your project here to show working prototypes, circuit designs, or UI if available.)_

---
