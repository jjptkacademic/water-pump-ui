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
  TIMER_TODAY: 'ptk/esp8266/timer/today-working',
  TIMER_DONE: 'ptk/esp8266/timer/done-today',
  DEBUG: 'ptk/esp8266/deug',

  // Publish (ส่งไป ESP8266)
  PUMP_CONTROL: 'ptk/esp8266/btn',
  AUTO_MODE: 'ptk/esp8266/set-auto',
  TIMER_MODE: 'ptk/esp8266/set-timer',
  DEBUG_MODE: 'ptk/esp8266/set-debug',
  TIMER_START: 'ptk/esp8266/timerstart',
  TIMER_STOP: 'ptk/esp8266/timerstop',
  TIMER_MON: 'ptk/esp8266/timer/mon',
  TIMER_TUE: 'ptk/esp8266/timer/tue',
  TIMER_WED: 'ptk/esp8266/timer/wed',
  TIMER_THU: 'ptk/esp8266/timer/thu',
  TIMER_FRI: 'ptk/esp8266/timer/fri',
  TIMER_SAT: 'ptk/esp8266/timer/sat',
  TIMER_SUN: 'ptk/esp8266/timer/sun'
};
