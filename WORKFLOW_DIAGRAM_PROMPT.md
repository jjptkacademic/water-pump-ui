# 🎨 Prompt สำหรับสร้าง Workflow Diagram ระบบควบคุมปั๊มน้ำอัตโนมัติ

---

## 📋 Prompt สำหรับ AI (Copy ไปใช้ได้เลย)

```
สร้าง workflow diagram แสดงการทำงานของระบบควบคุมปั๊มน้ำอัตโนมัติ โดยมีรายละเอียดดังนี้:

## ภาพรวมระบบ
- Platform: ESP8266 (NodeMCU)
- Communication: MQTT over WiFi (TLS/SSL)
- Sensors: Digital Water Sensors (3 ตัว) + Ultrasonic Sensors HC-SR04 (2 ตัว)
- Actuator: Water Pump (1 ตัว)
- Control Modes: Manual, Auto, Timer (ทำงานพร้อมกันได้)

## Hardware Components

### Input Sensors:
1. **Digital Water Sensors (3 sensors):**
   - SensorinParkUp (D3) - ตรวจน้ำสวนเต็ม
   - SensorinSeaDown (D2) - ตรวจน้ำคลองหมด
   - SensorinSeaUp (D1) - ตรวจน้ำคลองเต็ม

2. **Ultrasonic Sensors HC-SR04 (2 sensors):**
   - ULTRASONIC_PARK (D0 TRIG, D5 ECHO) - วัดระดับน้ำสวน 0-100%
   - ULTRASONIC_PUB (D8 TRIG, D4 ECHO) - วัดระดับน้ำคลอง 0-100%

### Output:
- Water Pump (D7) - ปั๊มน้ำจากคลองไปสวน
- LED Green (A0) - แสดงสถานะ
- LED Red (D6) - แสดงสถานะ

## Control Modes (3 โหมด)

### 1. Manual Mode (Priority 3)
- ควบคุมผ่าน MQTT: ptk/esp8266/btn
- Payload: "Btn_ON" / "Btn_OFF"
- สามารถเปิด/ปิดปั๊มได้โดยตรง
- ยังต้องผ่าน Safety Check

### 2. Auto Mode (Priority 4)
- เปิด/ปิด: ptk/esp8266/set-auto
- ใช้ Ultrasonic Sensors ควบคุม
- เงื่อนไขเปิดปั๊ม:
  * Park water level < 30% (threshold ปรับได้)
  * Pub water level > 20% (threshold ปรับได้)
- เงื่อนไขหยุดปั๊ม:
  * Park water level >= 90% (threshold ปรับได้)
  * Pub water level < 20%
  * หรือ Safety sensors trigger

### 3. Timer Mode (Priority 2)
- เปิด/ปิด: ptk/esp8266/set-timer
- ตั้งเวลาเริ่ม/หยุด: HH:MM
- เลือกวัน: จันทร์-อาทิตย์
- ทำงานครั้งเดียวต่อวัน (ป้องกันทำงานซ้ำ)
- Reset flag เที่ยงคืนทุกวัน
- Manual reset: ptk/esp8266/timer/reset-flag

## Safety Logic (Priority 1 - สูงสุด)

### Safety Check (shouldStopPump):
1. **Digital Sensor - น้ำคลองหมด:**
   - IF seaDownStatus == 1 → STOP ทันที

2. **Digital Sensor - น้ำสวนเต็ม:**
   - IF parkUpStatus == 1 → STOP ทันที

3. **Ultrasonic - น้ำสวนเต็ม:**
   - IF ultrasonicPark >= 90% → STOP
   - (ทำงานก่อนถึง parkUp sensor)

4. **Ultrasonic - น้ำคลองต่ำ:**
   - IF ultrasonicPub < 20% → STOP
   - (ทำงานก่อนถึง seaDown sensor)

### Error Handling:
- Ultrasonic timeout: 30ms
- Error counter: 5 ครั้งติด → ใช้ digital sensors แทน
- Auto mode ปิดถ้า ultrasonic error
- Timer/Manual mode ยังทำงานได้ (ใช้ digital safety)

## Main Loop Flow

### SensorRead Task (200ms interval):
1. Read digital sensors (3 sensors)
2. Read ultrasonic sensors (2 sensors, ~60ms max)
3. Publish MQTT:
   - Digital: water-level-park, water-level-pub (0/1/2)
   - Ultrasonic: park/percent, pub/percent (0-100)
   - Status: OK/ERROR

### CheckPump Task (500ms interval):
Priority order:
1. **Safety Check** → IF fail → STOP pump
2. **Timer Mode** → IF active → RUN pump
3. **Manual Mode** → IF button ON → RUN pump
4. **Auto Mode** → IF conditions met → RUN pump
5. **Continue** → IF auto flag ON → RUN pump
6. **ELSE** → STOP pump

### Timer Check Task (500ms interval):
1. Check day change → Reset execution flag
2. IF timer mode disabled → Return false
3. Check current day enabled
4. Check time in range (start-stop)
5. Check safety conditions
6. Check NOT executed today
7. IF all pass → Set flag + Start pump

## MQTT Communication

### Subscribe Topics (Control):
- ptk/esp8266/btn - Manual control
- ptk/esp8266/set-auto - Auto mode enable
- ptk/esp8266/set-timer - Timer mode enable
- ptk/esp8266/timerstart/stop - Timer config
- ptk/esp8266/timer/mon-sun - Day selection
- ptk/esp8266/ultrasonic/threshold/* - Threshold config
- ptk/esp8266/timer/reset-flag - Reset timer

### Publish Topics (Status):
- ptk/esp8266/status - Pump status
- ptk/esp8266/water-level-* - Digital sensors
- ptk/esp8266/ultrasonic/*/percent - Water levels
- ptk/esp8266/ultrasonic/*/status - Sensor status
- ptk/esp8266/timer/executed-today - Timer status

## สร้าง Diagram แสดง:

1. **System Architecture:**
   - ESP8266 (center)
   - Sensors (left side)
   - Actuators (right side)
   - MQTT Broker (top)
   - WiFi connection

2. **Main Control Flow:**
   - Setup → Loop → Tasks
   - SensorRead → CheckPump → Control Pump
   - MQTT callback → Update settings

3. **Decision Tree for Pump Control:**
   - Safety Check (highest priority)
   - Timer Mode check
   - Manual Mode check
   - Auto Mode check
   - Continue check
   - Result: Pump ON/OFF

4. **Auto Mode Flowchart:**
   - Check ultrasonic sensors
   - Check thresholds
   - Compare park < start AND pub > min
   - Check park >= stop OR pub < min
   - Result: Start/Stop pump

5. **Timer Mode Flowchart:**
   - Check timer enabled
   - Check day of week
   - Check time range
   - Check safety
   - Check executed today
   - Result: Run pump (once per day)

6. **Safety System Layers:**
   - Layer 1: Digital hard stop (always active)
   - Layer 2: Ultrasonic soft stop
   - Layer 3: Error fallback
   - Show override logic

7. **MQTT Data Flow:**
   - App → Control topics → ESP8266
   - ESP8266 → Status topics → App
   - Show bidirectional communication

8. **State Machine:**
   - IDLE → MANUAL/AUTO/TIMER → RUNNING → STOPPED → IDLE
   - Show transitions and conditions

ใช้สี:
- 🔴 Red: Safety/Stop conditions
- 🟢 Green: Running/OK status
- 🟡 Yellow: Warning/Threshold
- 🔵 Blue: Control inputs
- ⚪ Gray: Sensors/Data

ใช้รูปแบบ:
- Flowchart สำหรับ logic flow
- Sequence diagram สำหรับ MQTT communication
- State diagram สำหรับ modes
- Architecture diagram สำหรับ hardware connections

สร้างให้ครบทั้ง 8 diagrams ข้างต้น และแสดงความสัมพันธ์ระหว่างส่วนต่างๆ อย่างชัดเจน
```

---

## 🎨 Prompt แยกตาม Diagram (ถ้าต้องการทีละอัน)

### 1. System Architecture Diagram

```
สร้าง System Architecture Diagram ของระบบควบคุมปั๊มน้ำอัตโนมัติ:

**ส่วนประกอบ:**

[ESP8266 NodeMCU] (กลาง)
├─ WiFi → [MQTT Broker HiveMQ Cloud]
├─ INPUT Sensors (ซ้าย):
│  ├─ Digital Sensors:
│  │  ├─ SensorinParkUp (D3) - น้ำสวนเต็ม
│  │  ├─ SensorinSeaDown (D2) - น้ำคลองหมด
│  │  └─ SensorinSeaUp (D1) - น้ำคลองเต็ม
│  └─ Ultrasonic Sensors (HC-SR04):
│     ├─ Park Tank: TRIG(D0) + ECHO(D5) → 0-100%
│     └─ Pub Tank: TRIG(D8) + ECHO(D4) → 0-100%
└─ OUTPUT Actuators (ขวา):
   ├─ Water Pump (D7) - ปั๊มน้ำ
   ├─ LED Green (A0) - สถานะ
   └─ LED Red (D6) - สถานะ

**MQTT Topics:**
- Subscribe: 16 control topics
- Publish: 14 status topics

**Power:**
- 5V VIN → HC-SR04 (2 ตัว)
- 3.3V → ESP8266 + Sensors

ใช้สี: 🔵 Input, 🟢 Output, ⚪ Communication, 🟡 Power
```

### 2. Main Control Flow

```
สร้าง Flowchart แสดง Main Control Logic:

START
↓
[Safety Check]
├─ seaDownStatus == 1? → YES → [STOP PUMP] → END
├─ parkUpStatus == 1? → YES → [STOP PUMP] → END
├─ ultrasonic park >= 90%? → YES → [STOP PUMP] → END
└─ ultrasonic pub < 20%? → YES → [STOP PUMP] → END
↓ NO (ปลอดภัย)
[Check Timer Mode]
├─ flag_timer_mode_enabled? → NO → Next
└─ YES → [In time range?]
    ├─ YES → [Executed today?]
    │   ├─ NO → [START PUMP] → Mark executed → END
    │   └─ YES → Next
    └─ NO → Next
↓
[Check Manual Mode]
├─ flag_trigBtn_start? → YES → [START PUMP] → END
└─ NO → Next
↓
[Check Auto Mode]
├─ flag_set_automatic_Check? → NO → Next
└─ YES → [Ultrasonic Check]
    ├─ Park < 30% AND Pub > 20%? → YES → [START PUMP] → END
    └─ NO → Next
↓
[Continue Running?]
├─ flag_autopump_on? → YES → [CONTINUE PUMP] → END
└─ NO → [STOP PUMP] → END

Priority: Safety > Timer > Manual > Auto > Continue

ใช้สี:
🔴 STOP conditions
🟢 START/CONTINUE
🟡 Checks
🔵 Modes
```

### 3. Auto Mode Detailed Flow

```
สร้าง Auto Mode Logic Flowchart:

[Auto Mode Enabled?]
↓ YES
[Check Ultrasonic Sensors]
├─ Park sensor OK? → NO → [Use Digital Fallback]
└─ Pub sensor OK? → NO → [Use Digital Fallback]
↓ BOTH OK
[Read Water Levels]
├─ Park Level = ultrasonicPark.getWaterLevelPercent()
└─ Pub Level = ultrasonicPub.getWaterLevelPercent()
↓
[shouldStartPumpUltrasonic()]
├─ Park < parkStartThreshold (30%)? → NO → Don't Start
├─ AND Pub > pubMinThreshold (20%)? → NO → Don't Start
└─ BOTH YES → [START PUMP] → flag_autopump_on = true
↓
[Pump Running...]
↓
[shouldStopPump()]
├─ Park >= parkStopThreshold (90%)? → YES → STOP
├─ OR Pub < pubMinThreshold (20%)? → YES → STOP
├─ OR Digital safety trigger? → YES → STOP
└─ NO → Continue
↓
[Threshold ปรับได้ผ่าน MQTT:]
- park-start: 0-100%
- park-stop: 0-100%
- pub-min: 0-100%

สี:
🟢 Start conditions
🔴 Stop conditions
🟡 Thresholds
🔵 Sensor readings
```

### 4. Timer Mode Flow

```
สร้าง Timer Mode Flowchart พร้อม Once-Per-Day Logic:

[Timer Mode Enabled?]
↓ YES
[Check Day Change]
├─ currentDay != lastTimerExecutedDay?
└─ YES → Reset flag_timer_executed_today = false
         Publish MQTT: "executed-today" = "NO"
↓
[Current Day Enabled?]
├─ dayOn_Select[currentDay] == 1?
└─ NO → Return false
↓ YES
[Current Time in Range?]
├─ currentTime >= startTime?
├─ AND currentTime < stopTime?
└─ NO → Return false
↓ YES
[Safety Check]
├─ seaDownStatus == 0? (มีน้ำคลอง)
├─ AND parkUpStatus == 0? (สวนไม่เต็ม)
└─ NO → Return false
↓ YES
[Executed Today Check]
├─ flag_timer_executed_today == true?
└─ YES → [ป้องกันทำงานซ้ำ] → Return false
↓ NO (ยังไม่ทำงานวันนี้)
[START PUMP]
├─ flag_timer_executed_today = true
├─ Publish MQTT: "executed-today" = "YES"
└─ Return true
↓
[When Stop?]
├─ น้ำเต็ม/หมด → Stop but flag ยังเป็น true
├─ เลยเวลา stopTime → Stop
└─ วันพรุ่งนี้ 00:00 → Reset flag

[Manual Reset Available:]
MQTT: ptk/esp8266/timer/reset-flag = "RESET"
→ flag_timer_executed_today = false

Timeline Example:
00:00 → Reset flag → "NO"
06:00 → Start pump → "YES"
06:30 → Water full, stop pump → flag still "YES"
07:00 → Water drops, but flag = "YES" → Don't start again ✓
00:00 next day → Reset flag → "NO"

สี:
🟢 Executed NO (พร้อมทำงาน)
🔴 Executed YES (ทำงานแล้ว)
🟡 Time checks
🔵 Safety checks
```

### 5. Safety System Layers

```
สร้าง Safety System Diagram แบบ Multi-Layer:

[Layer 1: DIGITAL HARD STOP] 🔴 (Priority สูงสุด)
├─ seaDownStatus == 1 → STOP (น้ำคลองหมด)
└─ parkUpStatus == 1 → STOP (น้ำสวนเต็ม)
   ↓ Always Active, Cannot Override

[Layer 2: ULTRASONIC SOFT STOP] 🟡 (ทำงานก่อน Layer 1)
├─ IF no error:
│  ├─ park >= 90% → STOP (ก่อนถึง parkUp sensor)
│  └─ pub < 20% → STOP (ก่อนน้ำคลองหมด)
└─ IF error → Disable, ใช้ Layer 1

[Layer 3: MODE CONTROL] 🔵
├─ Timer Mode → IF executed today → Prevent restart
├─ Auto Mode → IF ultrasonic error → Disable auto
└─ Manual Mode → Always available

[Layer 4: ERROR HANDLING] ⚪
├─ Ultrasonic timeout: 30ms
├─ Error counter: >= 5 → hasError() = true
├─ Fallback: Use last valid value (if < 5 errors)
└─ Recovery: Auto reset when sensor OK

[Override Logic:]
Digital sensors ALWAYS override ultrasonic
Timer/Manual can override Auto mode
Safety ALWAYS overrides all modes

[Fail-Safe Behavior:]
- Ultrasonic error → Auto mode disabled
- No sensors → Pump won't start (safe)
- MQTT disconnect → Last settings maintained
- Power loss → All flags reset to safe defaults

Diagram แบบ:
- Pyramid: Safety (bottom) → Control (top)
- Arrows: Show override directions
- Colors: Priority levels
```

### 6. MQTT Communication Flow

```
สร้าง Sequence Diagram แสดง MQTT Communication:

[Mobile App] ←→ [MQTT Broker] ←→ [ESP8266]

=== Scenario 1: Manual Control ===
App → Broker: PUBLISH ptk/esp8266/btn "Btn_ON"
Broker → ESP: RECEIVE topic + payload
ESP: handleCallback() → setButtonStatus(true)
ESP: checkPumpWorking() → flag_trigBtn_start = true
ESP: Safety check → PASS
ESP: openPump() → digitalWrite(Wather_Pump, HIGH)
ESP → Broker: PUBLISH ptk/esp8266/status "Led_ON"
ESP → Broker: PUBLISH ptk/esp8266/btn "Btn_ON"
Broker → App: UPDATE UI (Pump ON)

=== Scenario 2: Auto Mode ===
App → Broker: PUBLISH ptk/esp8266/set-auto "Auto_ON"
Broker → ESP: RECEIVE
ESP: flag_set_automatic_Check = true

[Every 200ms:]
ESP: Read ultrasonic sensors
ESP: Park = 25%, Pub = 65%
ESP → Broker: PUBLISH ptk/esp8266/ultrasonic/park/percent "25.0"
ESP → Broker: PUBLISH ptk/esp8266/ultrasonic/pub/percent "65.0"
Broker → App: UPDATE water level display

[Every 500ms:]
ESP: checkPumpWorking()
ESP: shouldStartPumpUltrasonic() → true (25% < 30% AND 65% > 20%)
ESP: openPump()
ESP → Broker: PUBLISH ptk/esp8266/status "Led_ON"
Broker → App: UPDATE pump status

=== Scenario 3: Timer Mode ===
App → Broker: PUBLISH ptk/esp8266/set-timer "Timer_ON"
App → Broker: PUBLISH ptk/esp8266/timerstart "06:30"
App → Broker: PUBLISH ptk/esp8266/timerstop "18:00"
App → Broker: PUBLISH ptk/esp8266/timer/mon "Mon_ON"
Broker → ESP: RECEIVE all settings
ESP: Save settings

[At 06:00:]
ESP: checkTimerPump() → Wait for 06:30

[At 06:30:]
ESP: checkTimerPump() → In range, day enabled
ESP: NOT executed today → START
ESP: flag_timer_executed_today = true
ESP → Broker: PUBLISH ptk/esp8266/timer/executed-today "YES"
ESP → Broker: PUBLISH ptk/esp8266/status "Led_ON"
Broker → App: UPDATE timer status + pump status

[At 07:00 (water drops):]
ESP: checkTimerPump() → executed today = YES → DON'T START
(ป้องกันทำงานซ้ำ)

=== Scenario 4: Threshold Update ===
App → Broker: PUBLISH ptk/esp8266/ultrasonic/threshold/park-stop "85"
Broker → ESP: RECEIVE
ESP: waterSensor->updateParkStopThreshold(85.0)
ESP → Broker: PUBLISH ptk/esp8266/debug "Park stop threshold updated"
Broker → App: Confirm update

Retained Messages: ALL topics use retained = true
→ New subscribers get last value immediately

สี:
🔵 App → Broker
🟢 Broker → ESP
🟡 ESP internal
🟠 ESP → Broker
⚪ Broker → App
```

### 7. State Machine Diagram

```
สร้าง State Machine สำหรับ Pump Control:

States:
1. [IDLE] - Pump off, waiting
2. [MANUAL_ON] - Manual control active
3. [AUTO_RUNNING] - Auto mode running
4. [TIMER_RUNNING] - Timer mode running
5. [SAFETY_STOP] - Stopped by safety

Transitions:

[IDLE]
├─ Manual button ON → [MANUAL_ON]
├─ Auto threshold met → [AUTO_RUNNING]
├─ Timer time reached → [TIMER_RUNNING]
└─ Safety trigger → Stay [IDLE]

[MANUAL_ON]
├─ Manual button OFF → [IDLE]
├─ Safety trigger → [SAFETY_STOP]
└─ Pump running...

[AUTO_RUNNING]
├─ Threshold stop met → [IDLE]
├─ Auto mode OFF → [IDLE]
├─ Safety trigger → [SAFETY_STOP]
└─ Pump running...

[TIMER_RUNNING]
├─ Time expired → [IDLE]
├─ Water full/empty → [IDLE] (flag stays true)
├─ Timer mode OFF → [IDLE]
├─ Safety trigger → [SAFETY_STOP]
└─ Pump running...

[SAFETY_STOP]
├─ Safety cleared + Manual ON → [MANUAL_ON]
├─ Safety cleared + Auto OK → [AUTO_RUNNING]
├─ Safety cleared + Timer OK → [TIMER_RUNNING]
└─ Safety cleared + No trigger → [IDLE]

Events:
- Button ON/OFF (MQTT)
- Auto mode ON/OFF (MQTT)
- Timer mode ON/OFF (MQTT)
- Threshold met (Sensor)
- Time reached (NTP)
- Safety trigger (Sensor)
- Safety cleared (Sensor)

Priority Resolution:
IF multiple modes active:
  Safety > Timer > Manual > Auto

สี:
⚪ IDLE
🔵 MANUAL_ON
🟢 AUTO_RUNNING
🟡 TIMER_RUNNING
🔴 SAFETY_STOP
```

### 8. Sensor Data Flow

```
สร้าง Data Flow Diagram สำหรับ Sensors:

=== Digital Sensors ===
[Physical Sensors (3)]
├─ SensorinParkUp (D3) → digitalWrite → parkUpStatus (0/1)
├─ SensorinSeaDown (D2) → digitalRead → seaDownStatus (0/1)
└─ SensorinSeaUp (D1) → digitalRead → seaUpStatus (0/1)
   ↓
[WaterSensor::read()] (200ms)
├─ Read all 3 sensors
└─ Update status variables
   ↓
[Calculate Water Level]
├─ Park: IF parkUp==1 → "2", ELSE "1"/"0" (from ultrasonic)
└─ Pub: IF seaUp==1 → "2", seaDown==0 → "1", seaDown==1 → "0"
   ↓
[MQTT Publish]
├─ ptk/esp8266/water-level-park: "0"/"1"/"2"
└─ ptk/esp8266/water-level-pub: "0"/"1"/"2"

=== Ultrasonic Sensors ===
[Physical HC-SR04 (2)]
├─ Park: TRIG(D0) + ECHO(D5)
└─ Pub: TRIG(D8) + ECHO(D4)
   ↓
[UltrasonicSensor::read()] (200ms)
├─ Send TRIG pulse (10μs)
├─ Wait ECHO response (timeout 30ms)
├─ Calculate distance = duration × 0.034 / 2
└─ Validate range (2-400 cm)
   ↓
[Error Handling]
├─ IF timeout/invalid → error_count++
├─ IF error_count >= 5 → hasError() = true
└─ ELSE use last_valid_distance
   ↓
[Calculate Percentage]
├─ level = (distance_empty - distance) / (distance_empty - distance_full) × 100
└─ Clamp to 0-100%
   ↓
[Change Detection]
├─ IF |current - last_published| > 2% → Publish
└─ ELSE skip (prevent MQTT spam)
   ↓
[MQTT Publish]
├─ ptk/esp8266/ultrasonic/park/percent: "45.2"
├─ ptk/esp8266/ultrasonic/park/status: "OK"/"ERROR"
├─ ptk/esp8266/ultrasonic/pub/percent: "67.8"
└─ ptk/esp8266/ultrasonic/pub/status: "OK"/"ERROR"

Timing:
- Digital: ~1ms total
- Ultrasonic: ~15-60ms (depends on distance + timeout)
- Total: ~100ms max per cycle
- Interval: 200ms → 50% utilization max

สี:
🔵 Digital sensors
🟢 Ultrasonic sensors
🟡 Processing
🟠 MQTT output
🔴 Error handling
```

---

## 💡 Tips สำหรับสร้าง Diagram

1. **ใช้ Mermaid.js** - Generate diagram จาก text
2. **ใช้ Draw.io / Excalidraw** - Drag & drop visual
3. **ใช้ PlantUML** - UML diagrams
4. **ใช้ Lucidchart** - Professional diagrams
5. **ใช้ AI Tools** - ChatGPT, Claude, Gemini

---

## 📚 Reference Files

- Main code: `C:\D\Project\Arduino73\Project\MiniProJect\`
- MQTT reference: `C:\D\Project\Arduino73\MQTT_TOPICS_REFERENCE.md`
- System docs: `C:\D\Project\Arduino73\SYSTEM_DOCUMENTATION.md`
- Ultrasonic plan: `C:\D\Project\Arduino73\ULTRASONIC_IMPLEMENTATION_PLAN.md`

---

**Created:** 2026-01-17
**For:** Workflow Diagram Generation
**AI Tools:** ChatGPT, Claude, Gemini, Mermaid, etc.
