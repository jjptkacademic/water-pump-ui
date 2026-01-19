// MQTT Broker Configuration
export const MQTT_CONFIG = {
  broker: 'wss://6555a54274d6454ab49e8eb9b94c325c.s1.eu.hivemq.cloud:8884/mqtt',
  options: {
    username: 'esp8266ptk',
    password: 'Aa12341234',
    clientId: 'web-dashboard-' + Math.random().toString(16).substr(2, 8),
    clean: true,
    reconnectPeriod: 5000,
    connectTimeout: 30000
  }
};

// MQTT Topics
export const TOPICS = {
  // Subscribe (รับจาก ESP8266)
  ULTRASONIC_PARK: 'ptk/esp8266/ultrasonic/park/percent',
  ULTRASONIC_PUB: 'ptk/esp8266/ultrasonic/pub/percent',
  WATER_LEVEL_PARK: 'ptk/esp8266/water-level-park',
  WATER_LEVEL_PUB: 'ptk/esp8266/water-level-pub',
  PUMP_STATUS: 'ptk/esp8266/status',
  BUTTON_STATUS: 'ptk/esp8266/btn',
  TIMER_EXECUTED: 'ptk/esp8266/timer/executed-today',  // ✅ แก้ชื่อ
  PONG: 'ptk/esp8266/pong',  // ✅ เพิ่ม Ping/Pong
  DEBUG: 'ptk/esp8266/deug',

  // Publish (ส่งไป ESP8266)
  PUMP_CONTROL: 'ptk/esp8266/btn',
  AUTO_MODE: 'ptk/esp8266/set-auto',
  TIMER_MODE: 'ptk/esp8266/set-timer',
  DEBUG_MODE: 'ptk/esp8266/set-debug',
  TIMER_START: 'ptk/esp8266/timerstart',
  TIMER_STOP: 'ptk/esp8266/timerstop',
  TIMER_MON: 'ptk/esp8266/timer/mon',
  TIMER_TUE: 'ptk/esp8266/timer/tues',  // ✅ แก้ tue → tues
  TIMER_WED: 'ptk/esp8266/timer/wed',
  TIMER_THU: 'ptk/esp8266/timer/thurs',  // ✅ แก้ thu → thurs
  TIMER_FRI: 'ptk/esp8266/timer/fri',
  TIMER_SAT: 'ptk/esp8266/timer/sat',
  TIMER_SUN: 'ptk/esp8266/timer/sun',

  // ✅ เพิ่ม Control Topics
  PING: 'ptk/esp8266/ping',
  TIMER_RESET: 'ptk/esp8266/timer/reset-flag',
  THRESHOLD_PARK_START: 'ptk/esp8266/ultrasonic/threshold/park-start',
  THRESHOLD_PARK_STOP: 'ptk/esp8266/ultrasonic/threshold/park-stop',
  THRESHOLD_PUB_MIN: 'ptk/esp8266/ultrasonic/threshold/pub-min'
};
