import { useState, useEffect } from 'react';
import { Container, Grid, AppBar, Toolbar, Typography, Box, Stack, Tabs, Tab } from '@mui/material';
import { ThemeProvider, CssBaseline } from '@mui/material';
import { darkTheme } from './theme/theme';
import HomeIcon from '@mui/icons-material/Home';
import TimerIcon from '@mui/icons-material/Timer';

import AnimatedWaterTank from './components/AnimatedWaterTank';
import PumpControl from './components/PumpControl';
import AutoModeSwitch from './components/AutoModeSwitch';
import TimerModeSwitch from './components/TimerModeSwitch';
import StatusIndicator from './components/StatusIndicator';
import TimerStatusIndicator from './components/TimerStatusIndicator';
import ConnectionStatus from './components/ConnectionStatus';
import TimerSettings from './components/TimerSettings';
import DaySelector from './components/DaySelector';
import DummyDataController from './components/DummyDataController';
import PingPongChecker from './components/PingPongChecker';
import TimerResetButton from './components/TimerResetButton';
import ThresholdSliders from './components/ThresholdSliders';

import { mqttService } from './services/mqttService';
import { TOPICS } from './services/config';

function App() {
  const [currentTab, setCurrentTab] = useState(0);
  const [connected, setConnected] = useState(false);
  const [useDummyData, setUseDummyData] = useState(false);

  const [ultrasonicPark, setUltrasonicPark] = useState(0);
  const [ultrasonicPub, setUltrasonicPub] = useState(0);
  const [parkUp, setParkUp] = useState(0);
  const [parkDown, setParkDown] = useState(0);
  const [pubUp, setPubUp] = useState(0);
  const [pubDown, setPubDown] = useState(0);

  const [pumpStatus, setPumpStatus] = useState(false);
  const [autoMode, setAutoMode] = useState(false);
  const [timerMode, setTimerMode] = useState(false);
  const [timerExecutedToday, setTimerExecutedToday] = useState(false);
  const [pongData, setPongData] = useState(null);

  const [timerStart, setTimerStart] = useState('06:00');
  const [timerStop, setTimerStop] = useState('08:00');
  const [selectedDays, setSelectedDays] = useState({
    mon: false,
    tue: false,
    wed: false,
    thu: false,
    fri: false,
    sat: false,
    sun: false
  });

  const handleMQTTMessage = (topic, message) => {
    if (useDummyData) return;

    switch(topic) {
      case TOPICS.ULTRASONIC_PARK:
        setUltrasonicPark(parseFloat(message));
        break;
      case TOPICS.ULTRASONIC_PUB:
        setUltrasonicPub(parseFloat(message));
        break;
      case TOPICS.WATER_LEVEL_PARK:
        const parkLevel = parseInt(message);
        setParkUp(parkLevel === 2 ? 1 : 0);
        setParkDown(parkLevel === 0 ? 1 : 0);
        break;
      case TOPICS.WATER_LEVEL_PUB:
        const pubLevel = parseInt(message);
        setPubUp(pubLevel === 2 ? 1 : 0);
        setPubDown(pubLevel === 0 ? 1 : 0);
        break;
      case TOPICS.PUMP_STATUS:
        setPumpStatus(message === 'Led_ON');
        break;
      case TOPICS.TIMER_EXECUTED:
        setTimerExecutedToday(message === 'YES');
        break;
      case TOPICS.PONG:
        setPongData(message);
        break;
      default:
        break;
    }
  };

  const handleDummyDataChange = (data) => {
    setUseDummyData(true);
    
    if (data.ultrasonicPark !== undefined) setUltrasonicPark(data.ultrasonicPark);
    if (data.ultrasonicPub !== undefined) setUltrasonicPub(data.ultrasonicPub);
    if (data.parkUp !== undefined) setParkUp(data.parkUp);
    if (data.parkDown !== undefined) setParkDown(data.parkDown);
    if (data.pubUp !== undefined) setPubUp(data.pubUp);
    if (data.pubDown !== undefined) setPubDown(data.pubDown);
    if (data.pumpStatus !== undefined) setPumpStatus(data.pumpStatus);
    if (data.timerExecutedToday !== undefined) setTimerExecutedToday(data.timerExecutedToday);
  };

  useEffect(() => {
    mqttService.connect({
      onConnect: () => setConnected(true),
      onDisconnect: () => setConnected(false),
      onMessage: handleMQTTMessage,
      onError: (error) => console.error(error)
    });

    return () => {
      mqttService.disconnect();
    };
  }, [useDummyData]);

  const handlePumpToggle = (turnOn) => {
    mqttService.publish(
      TOPICS.PUMP_CONTROL,
      turnOn ? 'Btn_ON' : 'Btn_OFF',
      true
    );
  };

  const handleAutoModeToggle = (enabled) => {
    setAutoMode(enabled);
    mqttService.publish(
      TOPICS.AUTO_MODE,
      enabled ? 'Auto_ON' : 'Auto_OFF',
      true
    );
  };

  const handleTimerModeToggle = (enabled) => {
    setTimerMode(enabled);
    mqttService.publish(
      TOPICS.TIMER_MODE,
      enabled ? 'Timer_ON' : 'Timer_OFF',
      true
    );
  };

  const handleTimeChange = (type, value) => {
    if (type === 'start') {
      setTimerStart(value);
      mqttService.publish(TOPICS.TIMER_START, value, true);
    } else {
      setTimerStop(value);
      mqttService.publish(TOPICS.TIMER_STOP, value, true);
    }
  };

  const handleDayToggle = (day, enabled) => {
    setSelectedDays(prev => ({ ...prev, [day]: enabled }));

    const topicMap = {
      mon: TOPICS.TIMER_MON,
      tue: TOPICS.TIMER_TUE,
      wed: TOPICS.TIMER_WED,
      thu: TOPICS.TIMER_THU,
      fri: TOPICS.TIMER_FRI,
      sat: TOPICS.TIMER_SAT,
      sun: TOPICS.TIMER_SUN
    };

    const messageMap = {
      mon: enabled ? 'Mon_ON' : 'Mon_OFF',
      tue: enabled ? 'Tues_ON' : 'Tues_OFF',
      wed: enabled ? 'Wed_ON' : 'Wed_OFF',
      thu: enabled ? 'Thurs_ON' : 'Thurs_OFF',
      fri: enabled ? 'Fri_ON' : 'Fri_OFF',
      sat: enabled ? 'Sat_ON' : 'Sat_OFF',
      sun: enabled ? 'Sun_ON' : 'Sun_OFF'
    };

    mqttService.publish(topicMap[day], messageMap[day], true);
  };

  const handlePing = () => {
    mqttService.publish(TOPICS.PING, 'PING', false);
  };

  const handleTimerReset = () => {
    mqttService.publish(TOPICS.TIMER_RESET, 'RESET', false);
  };

  const handleThresholdChange = (type, value) => {
    const topicMap = {
      'park-start': TOPICS.THRESHOLD_PARK_START,
      'park-stop': TOPICS.THRESHOLD_PARK_STOP,
      'pub-min': TOPICS.THRESHOLD_PUB_MIN
    };

    mqttService.publish(topicMap[type], value.toString(), true);
  };

  return (
    <ThemeProvider theme={darkTheme}>
      <CssBaseline />
      
      {/* Fixed AppBar - Full Width */}
      <AppBar position="fixed" elevation={2} sx={{ zIndex: 1200 }}>
        <Toolbar>
          <Typography variant="h5" sx={{ flexGrow: 1 }}>
            🌊 Water Pump Control Dashboard
          </Typography>
          <ConnectionStatus connected={connected} />
        </Toolbar>
      </AppBar>

      {/* Spacer for fixed AppBar */}
      <Toolbar />

      {/* Tabs Navigation */}
      <Box sx={{ borderBottom: 1, borderColor: 'divider', bgcolor: 'background.paper' }}>
        <Tabs
          value={currentTab}
          onChange={(e, newValue) => setCurrentTab(newValue)}
          centered
          sx={{ minHeight: 56 }}
        >
          <Tab icon={<HomeIcon />} label="Home" iconPosition="start" />
          <Tab icon={<TimerIcon />} label="Timer" iconPosition="start" />
        </Tabs>
      </Box>

      {/* Main Content - Full Page Flex Center */}
      <Box
        sx={{
          display: 'flex',
          justifyContent: 'center',
          width: '100%',
          minHeight: 'calc(100vh - 180px)'
        }}
      >
        <Box
          sx={{
            maxWidth: '1200px',
            width: '100%',
            padding: '24px'
          }}
        >

          {/* Status Bar */}
          <StatusIndicator status={pumpStatus} />

          {/* Timer Status Notice */}
          <TimerStatusIndicator
            timerExecutedToday={timerExecutedToday}
          />

          {/* Tab 1: Home */}
          {currentTab === 0 && (
            <Box sx={{ mt: 3 }}>
              {/* Tanks + Controls */}
              <Grid container spacing={3} justifyContent="center">

                {/* Tank Park */}
                <Grid size={{ xs: 12, md: 4 }}>
                  <AnimatedWaterTank
                    tankName="ถัง Park (ด้านบน)"
                    ultrasonicPercent={ultrasonicPark}
                    upStatus={parkUp}
                    downStatus={parkDown}
                  />
                </Grid>

                {/* Tank Pub */}
                <Grid size={{ xs: 12, md: 4 }}>
                  <AnimatedWaterTank
                    tankName="ถัง Pub (ด้านล่าง)"
                    ultrasonicPercent={ultrasonicPub}
                    upStatus={pubUp}
                    downStatus={pubDown}
                  />
                </Grid>

                {/* Controls */}
                <Grid size={{ xs: 12, md: 4 }}>
                  <Stack spacing={2}>
                    <PumpControl
                      pumpStatus={pumpStatus}
                      onToggle={handlePumpToggle}
                    />
                    <AutoModeSwitch
                      autoMode={autoMode}
                      onToggle={handleAutoModeToggle}
                    />
                  </Stack>
                </Grid>

              </Grid>

              {/* Threshold Sliders */}
              <Box sx={{ mt: 3 }}>
                <ThresholdSliders onThresholdChange={handleThresholdChange} />
              </Box>

              {/* Ping/Pong Checker */}
              <Box sx={{ mt: 3, display: 'flex', justifyContent: 'center' }}>
                <Box sx={{ width: '100%', maxWidth: 600 }}>
                  <PingPongChecker onPing={handlePing} pongData={pongData} />
                </Box>
              </Box>
            </Box>
          )}

          {/* Tab 2: Timer */}
          {currentTab === 1 && (
            <Box sx={{ mt: 3 }}>
              <Grid container spacing={3} justifyContent="center">

                {/* Timer Mode Switch */}
                <Grid size={{ xs: 12, md: 6 }}>
                  <TimerModeSwitch
                    timerMode={timerMode}
                    onToggle={handleTimerModeToggle}
                  />
                </Grid>

                {/* Timer Reset */}
                <Grid size={{ xs: 12, md: 6 }}>
                  <TimerResetButton
                    onReset={handleTimerReset}
                    timerExecutedToday={timerExecutedToday}
                  />
                </Grid>

                {/* Timer Settings */}
                <Grid size={{ xs: 12 }}>
                  <TimerSettings
                    startTime={timerStart}
                    stopTime={timerStop}
                    onTimeChange={handleTimeChange}
                  />
                </Grid>

                {/* Day Selector */}
                <Grid size={{ xs: 12 }}>
                  <DaySelector
                    selectedDays={selectedDays}
                    onDayToggle={handleDayToggle}
                  />
                </Grid>

              </Grid>
            </Box>
          )}

          {/* Footer */}
          <Box sx={{ mt: 5, mb: 2, textAlign: 'center', opacity: 0.6 }}>
            <Typography variant="caption">
              🌊 Water Pump Control System v2.0 | ESP8266 + MQTT | Made with ❤️
            </Typography>
          </Box>

        </Box>
      </Box>

      {/* Floating Dev Console */}
      <DummyDataController onDataChange={handleDummyDataChange} />
    </ThemeProvider>
  );
}

export default App;
