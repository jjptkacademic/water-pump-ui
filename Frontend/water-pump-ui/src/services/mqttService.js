import mqtt from 'mqtt';
import { MQTT_CONFIG, TOPICS } from './config';

export class MQTTService {
  constructor() {
    this.client = null;
    this.callbacks = {};
  }

  connect(callbacks) {
    this.callbacks = callbacks;

    console.log('🔌 Connecting to MQTT broker...');
    
    this.client = mqtt.connect(
      MQTT_CONFIG.broker,
      MQTT_CONFIG.options
    );

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

    this.client.on('reconnect', () => {
      console.log('🔄 MQTT Reconnecting...');
    });

    return this.client;
  }

  subscribeTopics() {
    const subscribeList = [
      TOPICS.ULTRASONIC_PARK,
      TOPICS.ULTRASONIC_PUB,
      TOPICS.WATER_LEVEL_PARK,
      TOPICS.WATER_LEVEL_PUB,
      TOPICS.PUMP_STATUS,
      TOPICS.BUTTON_STATUS,
      TOPICS.TIMER_EXECUTED,  // ✅ แก้ชื่อ
      TOPICS.PONG,  // ✅ เพิ่ม Pong
      TOPICS.DEBUG
    ];

    subscribeList.forEach(topic => {
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
      console.log('👋 MQTT Disconnected');
    }
  }
}

export const mqttService = new MQTTService();
