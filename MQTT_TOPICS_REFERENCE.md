# 📡 MQTT Topics Reference - ระบบควบคุมปั๊มน้ำอัตโนมัติ

## 📌 MQTT Broker Information

```
Server:   6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud
Port:     8883 (TLS/SSL)
Username: esp8266ptk
Password: Aa12341234
```

---

## 🔵 หมวด 1: CONTROL COMMANDS (App → ESP8266)

### 1.1 โหมดการทำงาน

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/set-auto` | `Auto_ON` / `Auto_OFF` | เปิด/ปิด Auto Mode (อัตโนมัติตาม sensor) |
| `ptk/esp8266/set-timer` | `Timer_ON` / `Timer_OFF` | เปิด/ปิด Timer Mode (ตั้งเวลา) |
| `ptk/esp8266/set-debug` | `D_ON` / `D_OFF` | เปิด/ปิด Debug Mode (Serial log) |
| `ptk/esp8266/btn` | `Btn_ON` / `Btn_OFF` | ควบคุมปั๊มด้วยมือ (Manual) |

### 1.2 ตั้งค่า Timer

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/timerstart` | `HH:MM` เช่น `"06:30"` | ตั้งเวลาเริ่มต้นปั๊ม |
| `ptk/esp8266/timerstop` | `HH:MM` เช่น `"18:00"` | ตั้งเวลาหยุดปั๊ม |

### 1.3 เลือกวันทำงาน Timer

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/timer/mon` | `Mon_ON` / `Mon_OFF` | เปิด/ปิด Timer ในวันจันทร์ |
| `ptk/esp8266/timer/tues` | `Tues_ON` / `Tues_OFF` | เปิด/ปิด Timer ในวันอังคาร |
| `ptk/esp8266/timer/wed` | `Wed_ON` / `Wed_OFF` | เปิด/ปิด Timer ในวันพุธ |
| `ptk/esp8266/timer/thurs` | `Thurs_ON` / `Thurs_OFF` | เปิด/ปิด Timer ในวันพฤหัสบดี |
| `ptk/esp8266/timer/fri` | `Fri_ON` / `Fri_OFF` | เปิด/ปิด Timer ในวันศุกร์ |
| `ptk/esp8266/timer/sat` | `Sat_ON` / `Sat_OFF` | เปิด/ปิด Timer ในวันเสาร์ |
| `ptk/esp8266/timer/sun` | `Sun_ON` / `Sun_OFF` | เปิด/ปิด Timer ในวันอาทิตย์ |

### 1.4 ตั้งค่า Ultrasonic Threshold

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/ultrasonic/threshold/park-start` | Float `"30"` | เกณฑ์เปิดปั๊ม (น้ำสวน < %) |
| `ptk/esp8266/ultrasonic/threshold/park-stop` | Float `"90"` | เกณฑ์หยุดปั๊ม (น้ำสวน >= %) |
| `ptk/esp8266/ultrasonic/threshold/pub-min` | Float `"20"` | น้ำขั้นต่ำในคลอง (>= %) |

### 1.5 คำสั่งพิเศษ

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/timer/reset-flag` | `RESET` | รีเซ็ต Timer flag ให้ทำงานซ้ำได้ในวันเดียวกัน |

---

## 🟢 หมวด 2: STATUS OUTPUT (ESP8266 → App)

### 2.1 สถานะปั๊มและระบบ

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/status` | `Led_ON` / `Led_OFF` | สถานะปั๊มน้ำ |
| `ptk/esp8266/btn` | `Btn_ON` / `Btn_OFF` | สถานะปุ่มควบคุม |
| `ptk/esp8266/debug` | Text message | ข้อความ debug ทั่วไป |
| `ptk/esp8266/deug` | Text message | ข้อความ debug (typo alt) |

### 2.2 สถานะ Timer

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/timer/today-working` | `Today_ON` / `Today_OFF` | วันนี้ timer เปิดหรือไม่ |
| `ptk/esp8266/timer/executed-today` | `YES` / `NO` | Timer ทำงานไปแล้วหรือยัง (ป้องกันทำงานซ้ำ) |

### 2.3 ระดับน้ำ - Digital Sensors

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/water-level-park` | `0` / `1` / `2` | ระดับน้ำสวน (0=ต่ำ, 1=กลาง, 2=เต็ม) |
| `ptk/esp8266/water-level-pub` | `0` / `1` / `2` | ระดับน้ำคลอง (0=หมด, 1=กลาง, 2=เต็ม) |

### 2.4 ระดับน้ำ - Ultrasonic Sensors (ความละเอียดสูง)

| Topic | Payload | คำอธิบาย |
|-------|---------|----------|
| `ptk/esp8266/ultrasonic/park/percent` | Float `"45.2"` | ระดับน้ำสวน % (อัปเดตเมื่อเปลี่ยน >2%) |
| `ptk/esp8266/ultrasonic/park/status` | `OK` / `ERROR` | สถานะเซนเซอร์ Ultrasonic สวน |
| `ptk/esp8266/ultrasonic/pub/percent` | Float `"67.8"` | ระดับน้ำคลอง % (อัปเดตเมื่อเปลี่ยน >2%) |
| `ptk/esp8266/ultrasonic/pub/status` | `OK` / `ERROR` | สถานะเซนเซอร์ Ultrasonic คลอง |

---

## 📊 การใช้งานตามฟีเจอร์

### 🎮 Manual Mode (ควบคุมด้วยมือ)

**เปิดปั๊ม:**
```
Publish → ptk/esp8266/btn: "Btn_ON"
Subscribe ← ptk/esp8266/status: "Led_ON"
Subscribe ← ptk/esp8266/btn: "Btn_ON"
```

**ปิดปั๊ม:**
```
Publish → ptk/esp8266/btn: "Btn_OFF"
Subscribe ← ptk/esp8266/status: "Led_OFF"
```

---

### 🤖 Auto Mode (อัตโนมัติตาม Sensor)

**เปิด Auto Mode:**
```
Publish → ptk/esp8266/set-auto: "Auto_ON"
```

**Logic:**
- เปิดปั๊มเมื่อ: น้ำสวน < 30% AND น้ำคลอง > 20%
- หยุดปั๊มเมื่อ: น้ำสวน >= 90% OR น้ำคลอง < 20%

**ตั้งค่า Threshold:**
```
Publish → ptk/esp8266/ultrasonic/threshold/park-start: "30"
Publish → ptk/esp8266/ultrasonic/threshold/park-stop: "90"
Publish → ptk/esp8266/ultrasonic/threshold/pub-min: "20"
```

**Monitor:**
```
Subscribe ← ptk/esp8266/ultrasonic/park/percent
Subscribe ← ptk/esp8266/ultrasonic/pub/percent
Subscribe ← ptk/esp8266/status
```

---

### ⏰ Timer Mode (ตั้งเวลา)

**เปิด Timer Mode:**
```
Publish → ptk/esp8266/set-timer: "Timer_ON"
```

**ตั้งเวลา:**
```
Publish → ptk/esp8266/timerstart: "06:30"
Publish → ptk/esp8266/timerstop: "18:00"
```

**เลือกวัน:**
```
Publish → ptk/esp8266/timer/mon: "Mon_ON"
Publish → ptk/esp8266/timer/tues: "Tues_ON"
Publish → ptk/esp8266/timer/wed: "Wed_ON"
... (เลือกวันที่ต้องการ)
```

**Monitor:**
```
Subscribe ← ptk/esp8266/timer/today-working
Subscribe ← ptk/esp8266/timer/executed-today
Subscribe ← ptk/esp8266/status
```

**Reset Timer (ทำงานซ้ำในวันเดียวกัน):**
```
Publish → ptk/esp8266/timer/reset-flag: "RESET"
```

---

### 💧 Monitor ระดับน้ำ

**Digital Sensors (พื้นฐาน):**
```
Subscribe ← ptk/esp8266/water-level-park
Subscribe ← ptk/esp8266/water-level-pub
```

**Ultrasonic Sensors (ละเอียด 0-100%):**
```
Subscribe ← ptk/esp8266/ultrasonic/park/percent
Subscribe ← ptk/esp8266/ultrasonic/park/status
Subscribe ← ptk/esp8266/ultrasonic/pub/percent
Subscribe ← ptk/esp8266/ultrasonic/pub/status
```

---

## 🎨 แนะนำโครงสร้าง UI

### หน้า Dashboard
```
┌─────────────────────────────────────┐
│ 🏠 ระบบควบคุมปั๊มน้ำ               │
├─────────────────────────────────────┤
│                                     │
│ 📊 ระดับน้ำ                         │
│ ┌─────────────┬─────────────┐      │
│ │ 🏞️ สวน      │ 🌊 คลอง     │      │
│ │ 45.2%       │ 67.8%       │      │
│ │ ████░░░░░░  │ ██████░░░░  │      │
│ │ ✅ OK       │ ✅ OK       │      │
│ └─────────────┴─────────────┘      │
│                                     │
│ 💧 สถานะปั๊ม: 🟢 ทำงาน            │
│                                     │
│ ⚙️ โหมด                             │
│ [Manual] [Auto ✓] [Timer]          │
└─────────────────────────────────────┘
```

**Subscribe Topics:**
- `ptk/esp8266/ultrasonic/park/percent`
- `ptk/esp8266/ultrasonic/pub/percent`
- `ptk/esp8266/ultrasonic/park/status`
- `ptk/esp8266/ultrasonic/pub/status`
- `ptk/esp8266/status`

---

### หน้า Manual Control
```
┌─────────────────────────────────────┐
│ 🎮 ควบคุมด้วยมือ                   │
├─────────────────────────────────────┤
│                                     │
│       [  เปิดปั๊ม  ] 🟢            │
│                                     │
│       [  ปิดปั๊ม  ] 🔴            │
│                                     │
│ สถานะ: ปั๊มทำงาน                   │
└─────────────────────────────────────┘
```

**Publish:**
- `ptk/esp8266/btn: "Btn_ON" / "Btn_OFF"`

**Subscribe:**
- `ptk/esp8266/status`
- `ptk/esp8266/btn`

---

### หน้า Auto Mode
```
┌─────────────────────────────────────┐
│ 🤖 โหมดอัตโนมัติ                   │
├─────────────────────────────────────┤
│ Auto Mode: [ON] 🟢                  │
│                                     │
│ ⚙️ ตั้งค่า Threshold                │
│ เปิดปั๊มเมื่อสวน: < 30%            │
│ ━━━━━━●━━━━━━━━━━━ 30%            │
│                                     │
│ หยุดปั๊มเมื่อสวน: >= 90%           │
│ ━━━━━━━━━━━━━━━●━━ 90%            │
│                                     │
│ น้ำคลองขั้นต่ำ: >= 20%             │
│ ━━━●━━━━━━━━━━━━━━ 20%            │
└─────────────────────────────────────┘
```

**Publish:**
- `ptk/esp8266/set-auto: "Auto_ON" / "Auto_OFF"`
- `ptk/esp8266/ultrasonic/threshold/park-start`
- `ptk/esp8266/ultrasonic/threshold/park-stop`
- `ptk/esp8266/ultrasonic/threshold/pub-min`

**Subscribe:**
- `ptk/esp8266/set-auto`

---

### หน้า Timer Mode
```
┌─────────────────────────────────────┐
│ ⏰ โหมดตั้งเวลา                     │
├─────────────────────────────────────┤
│ Timer Mode: [ON] 🟢                 │
│                                     │
│ เวลาเริ่ม: [06:30] 🕐              │
│ เวลาหยุด: [18:00] 🕐              │
│                                     │
│ เลือกวัน:                           │
│ [จ✓] [อ✓] [พ✓] [พฤ ] [ศ ] [ส ] [อา]│
│                                     │
│ สถานะวันนี้:                         │
│ ✅ ทำงานไปแล้ววันนี้                │
│                                     │
│ [  Reset Timer  ]                   │
└─────────────────────────────────────┘
```

**Publish:**
- `ptk/esp8266/set-timer: "Timer_ON" / "Timer_OFF"`
- `ptk/esp8266/timerstart: "HH:MM"`
- `ptk/esp8266/timerstop: "HH:MM"`
- `ptk/esp8266/timer/mon ~ sun: "Day_ON" / "Day_OFF"`
- `ptk/esp8266/timer/reset-flag: "RESET"`

**Subscribe:**
- `ptk/esp8266/set-timer`
- `ptk/esp8266/timer/today-working`
- `ptk/esp8266/timer/executed-today`

---

## 📋 สรุปทั้งหมด

**รวมทั้งหมด: 30 Topics**
- **Control Topics (App → ESP):** 16 topics
- **Status Topics (ESP → App):** 14 topics

**ทุก Topic ใช้ Retained Message = true** (เก็บค่าล่าสุด)

---

## 🔧 ตัวอย่าง Code

### JavaScript (Node.js / React Native)

```javascript
const mqtt = require('mqtt');

const client = mqtt.connect('mqtts://6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud:8883', {
  username: 'esp8266ptk',
  password: 'Aa12341234',
});

client.on('connect', () => {
  console.log('Connected to MQTT broker');

  // Subscribe to all status topics
  client.subscribe('ptk/esp8266/status');
  client.subscribe('ptk/esp8266/ultrasonic/park/percent');
  client.subscribe('ptk/esp8266/ultrasonic/pub/percent');
  client.subscribe('ptk/esp8266/timer/executed-today');
});

client.on('message', (topic, message) => {
  const payload = message.toString();
  console.log(`${topic}: ${payload}`);

  if (topic === 'ptk/esp8266/ultrasonic/park/percent') {
    const parkLevel = parseFloat(payload);
    console.log(`Park water level: ${parkLevel}%`);
  }
});

// เปิดปั๊ม
function turnPumpOn() {
  client.publish('ptk/esp8266/btn', 'Btn_ON');
}

// ปิดปั๊ม
function turnPumpOff() {
  client.publish('ptk/esp8266/btn', 'Btn_OFF');
}

// เปิด Auto Mode
function enableAutoMode() {
  client.publish('ptk/esp8266/set-auto', 'Auto_ON');
}

// ตั้งเวลา Timer
function setTimer(startTime, stopTime, days) {
  client.publish('ptk/esp8266/set-timer', 'Timer_ON');
  client.publish('ptk/esp8266/timerstart', startTime); // "06:30"
  client.publish('ptk/esp8266/timerstop', stopTime);   // "18:00"

  days.forEach(day => {
    client.publish(`ptk/esp8266/timer/${day}`, `${day}_ON`);
  });
}
```

---

### Python

```python
import paho.mqtt.client as mqtt

BROKER = "6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud"
PORT = 8883
USERNAME = "esp8266ptk"
PASSWORD = "Aa12341234"

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

    # Subscribe to topics
    client.subscribe("ptk/esp8266/status")
    client.subscribe("ptk/esp8266/ultrasonic/park/percent")
    client.subscribe("ptk/esp8266/ultrasonic/pub/percent")

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()
    print(f"{topic}: {payload}")

    if topic == "ptk/esp8266/ultrasonic/park/percent":
        park_level = float(payload)
        print(f"Park water level: {park_level}%")

client = mqtt.Client()
client.username_pw_set(USERNAME, PASSWORD)
client.tls_set()
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)

# เปิดปั๊ม
def turn_pump_on():
    client.publish("ptk/esp8266/btn", "Btn_ON")

# ปิดปั๊ม
def turn_pump_off():
    client.publish("ptk/esp8266/btn", "Btn_OFF")

client.loop_forever()
```

---

## ⚠️ หมายเหตุสำคัญ

1. **QoS**: ทุก topic ใช้ QoS 0 (default) และ Retained = true
2. **Payload Format**: ทุก payload เป็น String (ไม่ใช่ JSON)
3. **Float Values**: ส่งเป็น string เช่น `"45.2"` ไม่ใช่ `45.2`
4. **Case Sensitive**: Topic names และ payloads เป็น case-sensitive
5. **Connection**: ต้องใช้ TLS/SSL (port 8883)
6. **Typo Alert**: Topic `ptk/esp8266/deug` เป็น typo ของ "debug" แต่ใช้งานอยู่

---

## 📞 Support

**Project Location:** `C:\D\Project\Arduino73\Project\MiniProJect\`

**Key Files:**
- `MQTTHandler.cpp` - MQTT callback handlers
- `WaterSensor.cpp` - Sensor data publishing
- `PumpController.cpp` - Pump status publishing
- `TimeManager.cpp` - Timer management

---

**Created:** 2026-01-17
**Version:** 1.0
**Author:** Claude Code + User
