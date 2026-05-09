// ============================================
// SISTec IoT Application - Backend Server
// Express.js + JSON File Database
// ============================================

const express = require('express');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// ---- File Paths ----
const DB_FILE = path.join(__dirname, 'db.json');
const LCD_FILE = path.join(__dirname, 'lcd.txt');

// ---- Middleware ----
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

// ============================================
// DATABASE HELPER FUNCTIONS
// ============================================

// Read the database file
function readDB() {
  if (!fs.existsSync(DB_FILE)) {
    // Create empty database if it doesn't exist
    const emptyDB = { users: [], records: [], nextId: 1 };
    fs.writeFileSync(DB_FILE, JSON.stringify(emptyDB, null, 2));
    return emptyDB;
  }
  const data = fs.readFileSync(DB_FILE, 'utf-8');
  return JSON.parse(data);
}

// Save data to the database file
function writeDB(data) {
  fs.writeFileSync(DB_FILE, JSON.stringify(data, null, 2));
}

// Get current date and time in Asia/Kolkata timezone
function getIndiaDateTime() {
  const now = new Date();
  const options = { timeZone: 'Asia/Kolkata' };

  const time = now.toLocaleTimeString('en-IN', {
    timeZone: 'Asia/Kolkata',
    hour: '2-digit',
    minute: '2-digit',
    hour12: true
  });

  const date = now.toLocaleDateString('en-IN', {
    timeZone: 'Asia/Kolkata',
    day: '2-digit',
    month: '2-digit',
    year: 'numeric'
  }).replace(/\//g, '-');

  return { time, date };
}


// ============================================
// AUTH APIs
// ============================================

// Register API
app.post('/api/register', (req, res) => {
  const { name, email, password } = req.body;

  if (!name || !email || !password) {
    return res.json({ success: false, message: 'All fields are required.' });
  }

  const db = readDB();

  // Check if email already exists
  const existing = db.users.find(u => u.email === email);
  if (existing) {
    return res.json({ success: false, message: 'Email already registered.' });
  }

  // Add new user
  db.users.push({ name, email, password });
  writeDB(db);

  res.json({ success: true, message: 'Account created successfully.' });
});

// Login API
app.post('/api/login', (req, res) => {
  const { email, password } = req.body;

  if (!email || !password) {
    return res.json({ success: false, message: 'Email and password required.' });
  }

  const db = readDB();

  // Find user
  const user = db.users.find(u => u.email === email && u.password === password);
  if (!user) {
    return res.json({ success: false, message: 'Invalid email or password.' });
  }

  res.json({ success: true, name: user.name });
});


// ============================================
// SENSOR APIs
// ============================================

// API 1: Save Temperature & Humidity (for ESP8266)
// ESP8266 sends: GET /api/save?temperature=25&humidity=56
app.get('/api/save', (req, res) => {
  const temperature = req.query.temperature;
  const humidity = req.query.humidity;

  if (temperature === undefined || humidity === undefined) {
    return res.json({ success: false, message: 'temperature and humidity required.' });
  }

  const db = readDB();
  const { time, date } = getIndiaDateTime();

  // Create new record
  const record = {
    id: db.nextId,
    temperature: parseFloat(temperature),
    humidity: parseFloat(humidity),
    time,
    date
  };

  db.records.push(record);
  db.nextId += 1;
  writeDB(db);

  console.log(`Data saved → Temp: ${temperature}°C | Humidity: ${humidity}% | ${time} ${date}`);
  res.json({ success: true, message: 'Data saved successfully.' });
});

// Also allow POST for /api/save (optional)
app.post('/api/save', (req, res) => {
  const temperature = req.body.temperature || req.query.temperature;
  const humidity = req.body.humidity || req.query.humidity;

  if (temperature === undefined || humidity === undefined) {
    return res.json({ success: false, message: 'temperature and humidity required.' });
  }

  const db = readDB();
  const { time, date } = getIndiaDateTime();

  const record = {
    id: db.nextId,
    temperature: parseFloat(temperature),
    humidity: parseFloat(humidity),
    time,
    date
  };

  db.records.push(record);
  db.nextId += 1;
  writeDB(db);

  res.json({ success: true, message: 'Data saved successfully.' });
});

// Get latest sensor reading
app.get('/api/latest', (req, res) => {
  const db = readDB();
  if (db.records.length === 0) {
    return res.json({});
  }
  const latest = db.records[db.records.length - 1];
  res.json(latest);
});

// Get all records
app.get('/api/records', (req, res) => {
  const db = readDB();
  // Return records newest first
  const sorted = [...db.records].reverse();
  res.json(sorted);
});

// Delete a record by ID
app.delete('/api/delete/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const db = readDB();
  db.records = db.records.filter(r => r.id !== id);
  writeDB(db);
  res.json({ success: true });
});


// ============================================
// LCD APIs
// ============================================

// Save LCD text (from dashboard)
app.post('/api/lcd', (req, res) => {
  const { text } = req.body;

  if (!text) {
    return res.json({ success: false, message: 'Text is required.' });
  }

  // Only allow max 16 characters
  const safeText = text.substring(0, 16);

  // Overwrite lcd.txt with new text
  fs.writeFileSync(LCD_FILE, safeText);

  console.log(`LCD text updated: "${safeText}"`);
  res.json({ success: true, message: 'LCD text saved.' });
});

// API 2: Fetch LCD text (for ESP8266)
app.get('/api/lcd', (req, res) => {
  if (!fs.existsSync(LCD_FILE)) {
    return res.send('SISTec IoT');
  }
  const text = fs.readFileSync(LCD_FILE, 'utf-8');
  res.send(text);
});


// ============================================
// START SERVER
// ============================================
app.listen(PORT, () => {
  console.log(`SISTec IoT Server running on port ${PORT}`);
});
