# SISTec IoT Application 2026

A beginner-friendly IoT web application built with Node.js, Express, and HTML/Tailwind CSS.

---

## Project Structure

```
sistec-iot/
├── server.js           ← Backend (Node.js + Express)
├── package.json        ← Dependencies
├── db.json             ← JSON file database (auto-created)
├── lcd.txt             ← LCD text file (auto-created)
├── public/
│   ├── index.html      ← Login Page
│   ├── register.html   ← Registration Page
│   └── dashboard.html  ← IoT Dashboard
└── arduino/
    └── sistec_iot.ino  ← ESP8266 Arduino Code
```

---

## How to Run Locally

1. Install Node.js from https://nodejs.org
2. Open terminal in the project folder
3. Run:

```bash
npm install
node server.js
```

4. Open browser: http://localhost:3000

---

## Deploy on Render

1. Push this project to a GitHub repository
2. Go to https://render.com and create a free account
3. Click **New → Web Service**
4. Connect your GitHub repository
5. Set these settings:
   - **Build Command:** `npm install`
   - **Start Command:** `node server.js`
   - **Environment:** Node
6. Click **Deploy**
7. Render will give you a URL like: `https://your-app-name.onrender.com`

---

## API Reference

### Save Sensor Data (ESP8266 → Server)

```
GET https://your-app-name.onrender.com/api/save?temperature=25&humidity=56
```

Response:
```json
{ "success": true, "message": "Data saved successfully." }
```

---

### Fetch LCD Text (ESP8266 → Server)

```
GET https://your-app-name.onrender.com/api/lcd
```

Response:
```
Hello SISTec!
```

---

## Arduino Setup

### Required Libraries (install from Arduino Library Manager)
- **DHT sensor library** by Adafruit
- **Adafruit Unified Sensor**
- **LiquidCrystal I2C** by Frank de Brabander
- **ESP8266WiFi** (comes with ESP8266 board package)
- **ESP8266HTTPClient** (comes with ESP8266 board package)

### Hardware Connections

| Component | ESP8266 Pin |
|-----------|-------------|
| DHT11 Data | D5 |
| LCD SDA | D2 |
| LCD SCL | D1 |

### I2C Address: 0x27

### Steps to Upload Arduino Code

1. Open `arduino/sistec_iot.ino` in Arduino IDE
2. Change `YOUR_WIFI_NAME` and `YOUR_WIFI_PASSWORD`
3. Change `your-app-name.onrender.com` to your actual Render URL
4. Select Board: **NodeMCU 1.0 (ESP-12E Module)**
5. Upload the code

---

## Notes

- Database is stored in `db.json` file (auto-created on first run)
- LCD text is stored in `lcd.txt` file (overwritten on every submission)
- All times use **Asia/Kolkata (IST +5:30)** timezone
- Dashboard auto-refreshes every **10 seconds**
