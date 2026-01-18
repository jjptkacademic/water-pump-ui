# 🌊 Water Pump Control - Web Dashboard

## ภาพรวมโปรเจค

**ชื่อโปรเจค:** Water Pump Control Web Dashboard  
**Technology Stack:** React + Vite + MUI + MQTT.js  
**Platform:** Web Application (GitHub Pages)  
**Location:** `C:\D\Project\Arduino73\Frontend\`

---

## 🎯 Features

### Core Features
- 🌊 **Animated Water Tanks** - แสดงระดับน้ำแบบ real-time พร้อม wave animation
- 📊 **Dual Sensor Display** - แสดงทั้ง Ultrasonic (0-100%) และ Digital Sensors (Full/Half/Low)
- 🎛️ **Pump Control** - ควบคุมปั๊มด้วยมือผ่าน MQTT
- ⏰ **Timer Settings** - ตั้งเวลาเปิด-ปิดปั๊มอัตโนมัติ
- 📅 **Day Selector** - เลือกวันทำงาน (Mon-Sun)
- 🤖 **Auto Mode Toggle** - เปิด/ปิดโหมดอัตโนมัติ
- 🟢 **Real-time Status** - แสดงสถานะปั๊มและการเชื่อมต่อ MQTT

### Visual Features
- ✨ **CSS Wave Animation** - คลื่นน้ำเคลื่อนไหวสมจริง
- 🎨 **Dynamic Colors** - สีเปลี่ยนตามระดับน้ำ (เขียว/น้ำเงิน/ส้ม)
- 📱 **Responsive Design** - ใช้งานได้ทั้ง Desktop และ Mobile
- 🌙 **Dark Theme** - MUI Dark Mode

---

## 📁 Project Structure

```
Frontend/
│
├── 📦 package.json                    # Dependencies & scripts
├── 📦 package-lock.json               # Lock file
├── ⚙️ vite.config.js                  # Vite configuration
├── 🌐 index.html                      # Entry HTML
├── 📝 .gitignore                      # Git ignore
├── 📝 README.md                       # Project README
├── 📝 FRONTEND_PROJECT_PLAN.md        # This file
│
├── 📂 public/                         # Static assets
│   └── 🖼️ logo.png                    # App logo
│
└── 📂 src/                            # Source code
    ├── 🎯 main.jsx                    # React entry point
    ├── 🎨 App.jsx                     # Main app component
    ├── 🎨 App.css                     # Global styles
    │
    ├── 📂 components/                 # UI Components (11 files)
    │   ├── 💧 WaterTankCard.jsx              # Main tank container
    │   ├── 🌊 AnimatedWaterTank.jsx          # Animated water with waves
    │   ├── 📊 UltrasonicProgress.jsx         # Linear progress (0-100%)
    │   ├── 🔌 DigitalSensorChips.jsx         # Chip indicators (Full/Half/Low)
    │   ├── ⚖️ SensorComparisonAlert.jsx      # Dual sensor validation
    │   ├── 🎛️ PumpControl.jsx                # Manual control buttons
    │   ├── ⏰ TimerSettings.jsx              # Timer configuration
    │   ├── 📅 DaySelector.jsx                # Day selection chips
    │   ├── 🔘 AutoModeSwitch.jsx             # Auto mode toggle
    │   ├── 🟢 StatusIndicator.jsx            # Pump status display
    │   └── 🌐 ConnectionStatus.jsx           # MQTT connection badge
    │
    ├── 📂 services/                   # Business logic (2 files)
    │   ├── 📡 mqttService.js                 # MQTT connection & handlers
    │   └── 🔧 config.js                      # MQTT broker config
    │
    ├── 📂 hooks/                      # Custom React hooks (3 files)
    │   ├── 🎣 useMQTT.js                     # MQTT state management
    │   ├── 🎣 useWaterLevel.js               # Water level animation hook
    │   └── 🎣 useDualSensorSync.js           # Dual sensor validation logic
    │
    ├── 📂 utils/                      # Utility functions (2 files)
    │   ├── 🎨 waterColors.js                 # Color gradient helpers
    │   └── 📊 sensorValidation.js            # Sensor comparison logic
    │
    ├── 📂 styles/                     # CSS files (2 files)
    │   ├── 🌊 WaterTank.css                  # Wave animation styles
    │   └── 🎨 theme.css                      # Custom MUI theme overrides
    │
    └── 📂 theme/                      # MUI theme (1 file)
        └── 🎨 theme.js                       # MUI dark theme config
```

**Total Files:** ~25 files

---

## 🔧 Technology Stack

### Frontend Framework
```json
{
  "react": "^18.3.0",
  "react-dom": "^18.3.0",
  "vite": "^5.0.0"
}
```

### UI Library
```json
{
  "@mui/material": "^6.0.0",
  "@emotion/react": "^11.13.0",
  "@emotion/styled": "^11.13.0",
  "@mui/icons-material": "^6.0.0"
}
```

### Animation
```json
{
  "@react-spring/web": "^9.7.0"
}
```

### MQTT Communication
```json
{
  "mqtt": "^5.10.0"
}
```

### Utilities
```json
{
  "react-use": "^17.5.0"
}
```

---

## 📡 MQTT Integration

### Broker Configuration
```javascript
// services/config.js
export const MQTT_CONFIG = {
  broker: 'wss://6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud:8884/mqtt',
  options: {
    username: 'esp8266ptk',
    password: 'Aa12341234',
    clientId: 'web-dashboard-' + Math.random().toString(16).substr(2, 8),
    clean: true,
    reconnectPeriod: 5000
  }
};
```

### MQTT Topics

#### Subscribe (รับข้อมูลจาก ESP8266)
```javascript
const SUBSCRIBE_TOPICS = [
  'ptk/esp8266/ultrasonic/park/percent',     // Ultrasonic Park (0-100%)
  'ptk/esp8266/ultrasonic/pub/percent',      // Ultrasonic Pub (0-100%)
  'ptk/esp8266/water-level-park',            // Digital Park (0,1,2)
  'ptk/esp8266/water-level-pub',             // Digital Pub (0,1,2)
  'ptk/esp8266/status',                      // Pump status (Led_ON/OFF)
  'ptk/esp8266/btn',                         // Button status (Btn_ON/OFF)
  'ptk/esp8266/deug'                         // Debug messages
];
```

#### Publish (ส่งคำสั่งไป ESP8266)
```javascript
const PUBLISH_TOPICS = {
  pumpControl: 'ptk/esp8266/btn',            // Payload: "Btn_ON" | "Btn_OFF"
  autoMode: 'ptk/esp8266/set-auto',          // Payload: "Auto_ON" | "Auto_OFF"
  debug: 'ptk/esp8266/set-debug',            // Payload: "D_ON" | "D_OFF"
  timerStart: 'ptk/esp8266/timerstart',      // Payload: "06:30"
  timerStop: 'ptk/esp8266/timerstop',        // Payload: "08:00"
  dayMon: 'ptk/esp8266/timer/mon',           // Payload: "Mon_ON" | "Mon_OFF"
  dayTue: 'ptk/esp8266/timer/tue',
  dayWed: 'ptk/esp8266/timer/wed',
  dayThu: 'ptk/esp8266/timer/thu',
  dayFri: 'ptk/esp8266/timer/fri',
  daySat: 'ptk/esp8266/timer/sat',
  daySun: 'ptk/esp8266/timer/sun'
};
```

---

## 🎨 Component Examples

### 1. Animated Water Tank Component

```jsx
// components/AnimatedWaterTank.jsx
import { useSpring, animated } from '@react-spring/web';
import { Box, Typography } from '@mui/material';
import { useWaterLevel } from '../hooks/useWaterLevel';
import { useDualSensorSync } from '../hooks/useDualSensorSync';
import { getWaterGradient } from '../utils/waterColors';
import '../styles/WaterTank.css';

const AnimatedWaterTank = ({ 
  tankName,
  ultrasonicPercent,
  upStatus,
  downStatus 
}) => {
  // Sync dual sensors
  const { displayLevel, sensorStatus } = useDualSensorSync(
    ultrasonicPercent,
    { upStatus, downStatus }
  );

  // Animate water level
  const { animatedLevel, isChanging } = useWaterLevel(displayLevel);

  // Wave speed (faster when changing)
  const waveSpeed = isChanging ? '3s' : '8s';

  return (
    <Box className="tank-container">
      <Typography variant="h6" gutterBottom>
        {tankName}
      </Typography>

      {/* Tank Shell */}
      <Box className="tank-shell">
        
        {/* Sensor Markers */}
        <Box className="sensor-markers">
          <Box className={`marker full ${upStatus ? 'active' : ''}`}>
            <span>▲ Full</span>
          </Box>
          <Box className="marker half">
            <span>● Half</span>
          </Box>
          <Box className={`marker low ${downStatus ? 'active' : ''}`}>
            <span>▼ Low</span>
          </Box>
        </Box>

        {/* Animated Water */}
        <animated.div
          className="water"
          style={{
            height: animatedLevel.to(h => `${h}%`),
            background: getWaterGradient(displayLevel),
            '--wave-speed': waveSpeed
          }}
        >
          {/* Wave Layers */}
          <div className="wave wave1" />
          <div className="wave wave2" />
          <div className="wave wave3" />
        </animated.div>

        {/* Percentage Display */}
        <Typography className="level-text" variant="h4">
          {displayLevel.toFixed(1)}%
        </Typography>
      </Box>

      {/* Status Badge */}
      <SensorStatusBadge status={sensorStatus} />
    </Box>
  );
};

export default AnimatedWaterTank;
```

---

### 2. Water Level Animation Hook

```jsx
// hooks/useWaterLevel.js
import { useState, useEffect } from 'react';
import { useSpring } from '@react-spring/web';

export const useWaterLevel = (mqttLevel) => {
  const [currentLevel, setCurrentLevel] = useState(0);
  const [previousLevel, setPreviousLevel] = useState(0);

  // Smooth animation with React Spring
  const waterAnimation = useSpring({
    from: { height: previousLevel },
    to: { height: currentLevel },
    config: { 
      tension: 50,    // ความเร็ว
      friction: 14,   // ความนุ่มนวล
      mass: 1         // น้ำหนัก (realistic water movement)
    },
    onRest: () => {
      console.log('Water level stabilized at', currentLevel, '%');
    }
  });

  // Update when MQTT sends new value
  useEffect(() => {
    if (mqttLevel !== currentLevel) {
      setPreviousLevel(currentLevel);
      setCurrentLevel(mqttLevel);
    }
  }, [mqttLevel]);

  return {
    animatedLevel: waterAnimation.height,
    currentLevel,
    previousLevel,
    isChanging: currentLevel !== previousLevel
  };
};
```

---

### 3. Dual Sensor Sync Hook

```jsx
// hooks/useDualSensorSync.js
import { useState, useEffect } from 'react';

export const useDualSensorSync = (ultrasonicPercent, digitalSensors) => {
  const [displayLevel, setDisplayLevel] = useState(0);
  const [sensorStatus, setSensorStatus] = useState('syncing');

  useEffect(() => {
    const { upStatus, downStatus } = digitalSensors;
    let level = ultrasonicPercent;

    // Case 1: Ultrasonic ไม่พร้อมใช้งาน
    if (ultrasonicPercent === null || ultrasonicPercent < 0) {
      // Fallback to Digital Sensors
      if (upStatus === 1) {
        level = 100;
      } else if (upStatus === 0 && downStatus === 0) {
        level = 50;
      } else if (downStatus === 1) {
        level = 0;
      }
      setSensorStatus('fallback-digital');
    }
    
    // Case 2: ทั้ง 2 ระบบขัดแย้งกัน
    else if (
      (ultrasonicPercent > 80 && upStatus === 0) ||
      (ultrasonicPercent < 20 && downStatus === 0)
    ) {
      setSensorStatus('mismatch-warning');
      // ใช้ค่า Ultrasonic แต่แสดง warning
    }
    
    // Case 3: ทั้ง 2 ระบบสอดคล้องกัน
    else {
      setSensorStatus('synced');
    }

    setDisplayLevel(level);
  }, [ultrasonicPercent, digitalSensors]);

  return { displayLevel, sensorStatus };
};
```

---

### 4. MQTT Service

```jsx
// services/mqttService.js
import mqtt from 'mqtt';
import { MQTT_CONFIG } from './config';

export class MQTTService {
  constructor() {
    this.client = null;
    this.callbacks = {};
  }

  connect(callbacks) {
    this.callbacks = callbacks;

    // Connect to HiveMQ Cloud
    this.client = mqtt.connect(
      MQTT_CONFIG.broker, 
      MQTT_CONFIG.options
    );

    // Event handlers
    this.client.on('connect', () => {
      console.log('✅ MQTT Connected');
      this.subscribeTopics();
      if (this.callbacks.onConnect) {
        this.callbacks.onConnect();
      }
    });

    this.client.on('message', (topic, message) => {
      const payload = message.toString();
      console.log(`📨 ${topic}: ${payload}`);
      
      if (this.callbacks.onMessage) {
        this.callbacks.onMessage(topic, payload);
      }
    });

    this.client.on('error', (error) => {
      console.error('❌ MQTT Error:', error);
      if (this.callbacks.onError) {
        this.callbacks.onError(error);
      }
    });

    this.client.on('close', () => {
      console.log('🔌 MQTT Disconnected');
      if (this.callbacks.onDisconnect) {
        this.callbacks.onDisconnect();
      }
    });

    return this.client;
  }

  subscribeTopics() {
    const topics = [
      'ptk/esp8266/ultrasonic/park/percent',
      'ptk/esp8266/ultrasonic/pub/percent',
      'ptk/esp8266/water-level-park',
      'ptk/esp8266/water-level-pub',
      'ptk/esp8266/status',
      'ptk/esp8266/btn'
    ];

    topics.forEach(topic => {
      this.client.subscribe(topic, (err) => {
        if (err) {
          console.error(`❌ Subscribe failed: ${topic}`);
        } else {
          console.log(`✅ Subscribed: ${topic}`);
        }
      });
    });
  }

  publish(topic, message, retained = false) {
    if (this.client && this.client.connected) {
      this.client.publish(topic, message, { retain: retained });
      console.log(`📤 Published: ${topic} = ${message}`);
    } else {
      console.error('❌ MQTT not connected');
    }
  }

  disconnect() {
    if (this.client) {
      this.client.end();
      console.log('👋 MQTT Disconnected gracefully');
    }
  }
}

// Singleton instance
export const mqttService = new MQTTService();
```

---

### 5. Main App Component

```jsx
// App.jsx
import { useState, useEffect } from 'react';
import { Container, Grid, AppBar, Toolbar, Typography, Box } from '@mui/material';
import { ThemeProvider, createTheme } from '@mui/material/styles';
import CssBaseline from '@mui/material/CssBaseline';

import AnimatedWaterTank from './components/AnimatedWaterTank';
import PumpControl from './components/PumpControl';
import TimerSettings from './components/TimerSettings';
import DaySelector from './components/DaySelector';
import AutoModeSwitch from './components/AutoModeSwitch';
import StatusIndicator from './components/StatusIndicator';
import ConnectionStatus from './components/ConnectionStatus';

import { mqttService } from './services/mqttService';

// Dark theme
const darkTheme = createTheme({
  palette: {
    mode: 'dark',
    primary: { main: '#2196F3' },
    secondary: { main: '#4CAF50' }
  }
});

function App() {
  // MQTT Connection
  const [connected, setConnected] = useState(false);

  // Water Levels
  const [ultrasonicPark, setUltrasonicPark] = useState(0);
  const [ultrasonicPub, setUltrasonicPub] = useState(0);
  const [parkUp, setParkUp] = useState(0);
  const [parkDown, setParkDown] = useState(0);
  const [pubUp, setPubUp] = useState(0);
  const [pubDown, setPubDown] = useState(0);

  // Pump Status
  const [pumpStatus, setPumpStatus] = useState(false);
  const [autoMode, setAutoMode] = useState(false);

  // MQTT Message Handler
  const handleMQTTMessage = (topic, message) => {
    switch(topic) {
      // Ultrasonic Sensors (0-100%)
      case 'ptk/esp8266/ultrasonic/park/percent':
        setUltrasonicPark(parseFloat(message));
        break;
      case 'ptk/esp8266/ultrasonic/pub/percent':
        setUltrasonicPub(parseFloat(message));
        break;
      
      // Digital Sensors (0, 1, 2)
      case 'ptk/esp8266/water-level-park':
        const parkLevel = parseInt(message);
        setParkUp(parkLevel === 2 ? 1 : 0);
        setParkDown(parkLevel === 0 ? 1 : 0);
        break;
      case 'ptk/esp8266/water-level-pub':
        const pubLevel = parseInt(message);
        setPubUp(pubLevel === 2 ? 1 : 0);
        setPubDown(pubLevel === 0 ? 1 : 0);
        break;

      // Pump Status
      case 'ptk/esp8266/status':
        setPumpStatus(message === 'Led_ON');
        break;

      // Button Status
      case 'ptk/esp8266/btn':
        // Update UI if needed
        break;
    }
  };

  // Connect MQTT on mount
  useEffect(() => {
    mqttService.connect({
      onConnect: () => setConnected(true),
      onDisconnect: () => setConnected(false),
      onMessage: handleMQTTMessage,
      onError: (error) => console.error(error)
    });

    // Cleanup on unmount
    return () => {
      mqttService.disconnect();
    };
  }, []);

  // Control Functions
  const handlePumpToggle = (turnOn) => {
    mqttService.publish(
      'ptk/esp8266/btn', 
      turnOn ? 'Btn_ON' : 'Btn_OFF',
      true
    );
  };

  const handleAutoModeToggle = (enabled) => {
    setAutoMode(enabled);
    mqttService.publish(
      'ptk/esp8266/set-auto',
      enabled ? 'Auto_ON' : 'Auto_OFF',
      true
    );
  };

  return (
    <ThemeProvider theme={darkTheme}>
      <CssBaseline />
      
      {/* App Bar */}
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h5" sx={{ flexGrow: 1 }}>
            🌊 Water Pump Control Dashboard
          </Typography>
          <ConnectionStatus connected={connected} />
        </Toolbar>
      </AppBar>

      {/* Main Content */}
      <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
        
        {/* Water Tanks */}
        <Grid container spacing={3}>
          <Grid item xs={12} md={6}>
            <AnimatedWaterTank
              tankName="ถัง Park (ด้านบน)"
              ultrasonicPercent={ultrasonicPark}
              upStatus={parkUp}
              downStatus={parkDown}
            />
          </Grid>
          <Grid item xs={12} md={6}>
            <AnimatedWaterTank
              tankName="ถัง Pub (ด้านล่าง)"
              ultrasonicPercent={ultrasonicPub}
              upStatus={pubUp}
              downStatus={pubDown}
            />
          </Grid>
        </Grid>

        {/* Control Panel */}
        <Grid container spacing={3} sx={{ mt: 2 }}>
          <Grid item xs={12}>
            <StatusIndicator status={pumpStatus} />
          </Grid>
          <Grid item xs={12} md={6}>
            <PumpControl 
              pumpStatus={pumpStatus}
              onToggle={handlePumpToggle}
            />
          </Grid>
          <Grid item xs={12} md={6}>
            <AutoModeSwitch
              autoMode={autoMode}
              onToggle={handleAutoModeToggle}
            />
          </Grid>
        </Grid>

        {/* Timer Settings */}
        <Box sx={{ mt: 4 }}>
          <TimerSettings />
        </Box>

        {/* Day Selector */}
        <Box sx={{ mt: 3 }}>
          <DaySelector />
        </Box>

      </Container>
    </ThemeProvider>
  );
}

export default App;
```

---

### 6. Wave Animation CSS

```css
/* styles/WaterTank.css */

.tank-container {
  padding: 20px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 16px;
  backdrop-filter: blur(10px);
}

.tank-shell {
  position: relative;
  width: 100%;
  height: 400px;
  border: 3px solid #424242;
  border-radius: 12px;
  overflow: hidden;
  background: linear-gradient(
    180deg,
    rgba(33, 33, 33, 0.8) 0%,
    rgba(66, 66, 66, 0.8) 100%
  );
  box-shadow: 
    inset 0 0 20px rgba(0, 0, 0, 0.5),
    0 4px 20px rgba(0, 0, 0, 0.3);
}

/* Sensor Markers */
.sensor-markers {
  position: absolute;
  top: 0;
  right: 10px;
  height: 100%;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  padding: 20px 0;
  z-index: 10;
}

.marker {
  display: flex;
  align-items: center;
  font-size: 14px;
  color: #999;
  transition: all 0.3s ease;
}

.marker.active {
  color: #4CAF50;
  font-weight: bold;
  transform: scale(1.2);
}

.marker span {
  margin-left: 8px;
  padding: 4px 8px;
  background: rgba(0, 0, 0, 0.6);
  border-radius: 4px;
}

/* Animated Water */
.water {
  position: absolute;
  bottom: 0;
  left: 0;
  width: 100%;
  background: linear-gradient(180deg, #4FC3F7 0%, #0277BD 100%);
  transition: height 0.3s ease;
  overflow: hidden;
}

/* Wave Layers */
.wave {
  position: absolute;
  top: -50%;
  left: -50%;
  width: 200%;
  height: 200%;
  opacity: 0.6;
  background: radial-gradient(
    ellipse at center,
    rgba(255, 255, 255, 0.3) 0%,
    transparent 50%
  );
}

.wave1 {
  animation: wave var(--wave-speed, 8s) infinite linear;
}

.wave2 {
  animation: wave calc(var(--wave-speed, 8s) * 0.8) infinite linear reverse;
  opacity: 0.4;
}

.wave3 {
  animation: wave calc(var(--wave-speed, 8s) * 1.2) infinite linear;
  opacity: 0.3;
}

@keyframes wave {
  0% {
    transform: translate(-50%, -75%) rotate(0deg);
  }
  100% {
    transform: translate(-50%, -75%) rotate(360deg);
  }
}

/* Level Text */
.level-text {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  color: #fff;
  font-weight: bold;
  text-shadow: 
    0 2px 4px rgba(0, 0, 0, 0.5),
    0 0 10px rgba(255, 255, 255, 0.3);
  z-index: 5;
  pointer-events: none;
}

/* Responsive */
@media (max-width: 768px) {
  .tank-shell {
    height: 300px;
  }
  
  .marker {
    font-size: 12px;
  }
}
```

---

### 7. Water Color Utility

```javascript
// utils/waterColors.js

/**
 * Get water gradient based on level percentage
 * @param {number} level - Water level (0-100%)
 * @returns {string} CSS gradient string
 */
export const getWaterGradient = (level) => {
  if (level > 70) {
    // Green - Full/High
    return 'linear-gradient(180deg, #66BB6A 0%, #2E7D32 100%)';
  }
  if (level > 30) {
    // Blue - Normal
    return 'linear-gradient(180deg, #42A5F5 0%, #1565C0 100%)';
  }
  // Orange/Red - Low
  return 'linear-gradient(180deg, #FF9800 0%, #E65100 100%)';
};

/**
 * Get status color for badges/chips
 * @param {number} level - Water level
 * @returns {string} MUI color name
 */
export const getStatusColor = (level) => {
  if (level > 70) return 'success';
  if (level > 30) return 'primary';
  return 'warning';
};

/**
 * Get glow effect for low water warning
 * @param {number} level - Water level
 * @returns {object} CSS box-shadow object
 */
export const getWaterGlow = (level) => {
  if (level < 20) {
    return {
      boxShadow: '0 0 20px rgba(255, 152, 0, 0.6), 0 0 40px rgba(255, 152, 0, 0.4)',
      animation: 'pulse 2s infinite'
    };
  }
  return {};
};
```

---

## 🚀 Build & Deploy

### Development
```bash
# Install dependencies
npm install

# Run dev server
npm run dev

# Open browser: http://localhost:5173
```

### Build for Production
```bash
# Build static files
npm run build

# Preview production build
npm run preview
```

### Deploy to GitHub Pages
```bash
# Install gh-pages
npm install -D gh-pages

# Add to package.json scripts:
# "deploy": "npm run build && gh-pages -d dist"

# Deploy
npm run deploy

# URL: https://<username>.github.io/Arduino73/
```

### Vite Config for GitHub Pages
```javascript
// vite.config.js
import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
  plugins: [react()],
  base: '/Arduino73/', // Your repo name
  build: {
    outDir: 'dist',
    sourcemap: false
  }
});
```

---

## 📊 Data Flow Architecture

```
┌─────────────────────────────────────────────────────┐
│                    ESP8266                          │
│  - Ultrasonic Sensors (HC-SR04 x2)                 │
│  - Digital Water Sensors (x4)                      │
│  - Water Pump Controller                           │
└──────────────────┬──────────────────────────────────┘
                   │ MQTT Publish
                   ▼
┌─────────────────────────────────────────────────────┐
│           HiveMQ Cloud Broker                       │
│  wss://...s1.eu.hivemq.cloud:8884/mqtt             │
└──────────────────┬──────────────────────────────────┘
                   │ WebSocket (MQTT.js)
                   ▼
┌─────────────────────────────────────────────────────┐
│              Web Dashboard (React)                  │
├─────────────────────────────────────────────────────┤
│  1. mqttService.connect()                          │
│  2. Subscribe topics                               │
│  3. onMessage → handleMQTTMessage()                │
│  4. Update React state                             │
│  5. useDualSensorSync() → validate                 │
│  6. useWaterLevel() → animate                      │
│  7. AnimatedWaterTank renders                      │
└─────────────────────────────────────────────────────┘
                   │ User Actions
                   ▼
┌─────────────────────────────────────────────────────┐
│           Control Components                        │
│  - PumpControl → publish(btn, "Btn_ON")           │
│  - TimerSettings → publish(timerstart, "06:30")   │
│  - DaySelector → publish(timer/mon, "Mon_ON")     │
└──────────────────┬──────────────────────────────────┘
                   │ MQTT Publish
                   ▼
              Back to ESP8266
```

---

## 🎯 Implementation Checklist

### Phase 1: Setup (Day 1)
- [ ] Create Vite + React project
- [ ] Install dependencies (MUI, MQTT, React Spring)
- [ ] Setup folder structure
- [ ] Configure Vite for GitHub Pages

### Phase 2: Core Components (Day 2-3)
- [ ] Create AnimatedWaterTank component
- [ ] Implement wave CSS animation
- [ ] Create useWaterLevel hook
- [ ] Create useDualSensorSync hook
- [ ] Implement MQTT service

### Phase 3: UI Components (Day 4-5)
- [ ] PumpControl component
- [ ] TimerSettings component
- [ ] DaySelector component
- [ ] AutoModeSwitch component
- [ ] StatusIndicator component
- [ ] ConnectionStatus component

### Phase 4: Integration (Day 6)
- [ ] Connect MQTT to all components
- [ ] Test real-time updates
- [ ] Implement error handling
- [ ] Add loading states

### Phase 5: Polish (Day 7)
- [ ] Responsive design testing
- [ ] Animation optimization
- [ ] Add transitions
- [ ] Dark theme adjustments

### Phase 6: Deploy (Day 8)
- [ ] Build production version
- [ ] Test GitHub Pages deployment
- [ ] Update README
- [ ] Documentation complete

---

## 🐛 Troubleshooting

### MQTT Connection Issues
```javascript
// Enable debug logs
const client = mqtt.connect(broker, {
  ...options,
  log: console.log // Enable MQTT.js logging
});
```

### Animation Performance
```javascript
// Throttle MQTT updates
import { useThrottle } from 'react-use';

const throttledLevel = useThrottle(ultrasonicPercent, 500); // Update every 500ms
```

### WebSocket Blocked by CORS
- ✅ Use WSS (not WS) for secure connection
- ✅ HiveMQ Cloud supports WebSocket natively
- ✅ No CORS issues with proper broker setup

---

## 📝 Future Enhancements

### Version 2.0
- 📈 **Charts** - History graphs (Chart.js or MUI X Charts)
- 💾 **Local Storage** - Save timer settings
- 🔔 **Notifications** - Browser notifications on low water
- 📱 **PWA** - Install as mobile app
- 🌍 **Multi-language** - Thai/English support

### Version 3.0
- 📊 **Analytics Dashboard** - Water usage statistics
- 🤖 **AI Predictions** - Predict water refill times
- 📸 **Camera Integration** - View tank remotely
- 🎮 **Gesture Controls** - Voice/gesture pump control

---

## 📚 References

- [React Documentation](https://react.dev)
- [MUI Components](https://mui.com/material-ui/getting-started/)
- [React Spring](https://www.react-spring.dev)
- [MQTT.js](https://github.com/mqttjs/MQTT.js)
- [Vite Guide](https://vitejs.dev/guide/)
- [GitHub Pages](https://pages.github.com)

---

## 👨‍💻 Development Notes

### Performance Tips
- Use `React.memo()` for pure components
- Throttle MQTT updates (500ms recommended)
- Use CSS animations over JavaScript when possible
- Lazy load components with `React.lazy()`

### Security Notes
- **Never commit** MQTT credentials to Git
- Use environment variables (`.env`)
- GitHub Pages is public - use read-only MQTT user

### Testing
```bash
# Test MQTT connection
mosquitto_sub -h 6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud \
  -p 8883 -t "ptk/esp8266/#" -u esp8266ptk -P Aa12341234 --capath /etc/ssl/certs/
```

---

**Created:** 2026-01-15  
**Author:** GitHub Copilot CLI  
**Version:** 1.0.0
