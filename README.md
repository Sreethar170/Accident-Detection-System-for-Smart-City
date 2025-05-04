# 🚗 Accident Detection System for Smart City

A **Zone-Level Real-Time Accident Detection System** designed to improve road safety and emergency response in smart cities. This IoT-based project uses sensors, GPS, wireless communication, and simulated telecom towers to detect accidents, alert zone headquarters, and assist emergency response teams with real-time information.

---

## 🔍 Project Overview

This system detects vehicle collisions, provides a grace period for drivers to cancel false alerts, and sends detailed emergency data to the zone headquarters if necessary. Upon confirmation from HQ, the system provides ambulance ETA and distance back to the vehicle using display or audio.

---

### ✅ Key Features

- **Accident Detection:** Uses a mass-type sensor to detect collision impact.
- **Smart Delay Mechanism:** 
  - A 1-minute timer starts after an accident.
  - A voice alert prompts the driver.
  - The driver can cancel the alert using a stop button if they are conscious.
- **Automatic Alert System:**
  - If not cancelled, an alert is sent via the simulated tower (nRF24L01) network.
  - The alert reaches the zone HQ with detailed information.
- **Ambulance Distance & ETA Display:**
  - Once the HQ accepts the alert, it sends ambulance dispatch data back to the vehicle.
  - **Ambulance Distance** and **Estimated Time of Arrival (ETA)** are:
    - **Displayed** on an LCD or serial monitor.
    - Optionally **announced** via voice/audio module.
- **Fallback Location:** If GPS fails, Cell ID (Tower ID) is used to approximate location.
- **Voice Feedback:** Audio instructions and alerts help ensure clarity in emergency situations.

---

## 🧠 Technologies Used

- **Microcontroller:** Arduino Nano  
- **Wireless Communication:** nRF24L01 Module  
- **Location Tracking:** GPS Module  
- **Sensors:** Mass-Type Collision Sensor  
- **Display:** 16x2 LCD / Serial Monitor  
- **Audio Module:** DFPlayer Mini / Buzzer with pre-recorded voice  
- **Button Inputs:** Stop button for driver response  
- **Network Simulation:** Tower units using RF modules

---

## 📦 System Architecture

### 1. Vehicle Unit

- Monitors vehicle for collision impact using a mass sensor.
- On detection:
  - Starts a 1-minute countdown.
  - Plays voice alert to notify driver.
  - Driver can cancel with a button.
- If not cancelled:
  - Sends accident alert to Zone Tower.
  - Waits for HQ confirmation.
- After HQ accepts alert:
  - Displays/announces ambulance **distance** and **ETA**.

### 2. Zone Tower

- Receives data from Vehicle Unit.
- Transmits it to Zone Headquarters.
- Sends ambulance details back to the vehicle.

### 3. Zone Headquarters

- Displays all incoming alert details.
- Locates accident via GPS or Cell ID.
- Dispatches ambulance and calculates ETA.
- Sends ETA info back to vehicle.

---

## 🧾 Alert Message Details

The alert message includes:

- Vehicle Owner Name  
- Driver Name  
- Driver Age & Blood Group  
- Phone Number  
- Vehicle Info (Number, Type, etc.)  
- Home Address  
- Current Location (GPS or Tower ID)  

---
