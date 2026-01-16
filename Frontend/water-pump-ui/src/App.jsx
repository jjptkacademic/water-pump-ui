import { useState, useEffect } from 'react';
import { Container, Grid, AppBar, Toolbar, Typography, Box, Stack } from '@mui/material';
import { ThemeProvider, CssBaseline } from '@mui/material';
import { darkTheme } from './theme/theme';

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

import { mqttService } from './services/mqttService';
import { TOPICS } from './services/config';

function App() {
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
  const [timerWorking, setTimerWorking] = useState(false);
  const [timerDoneToday, setTimerDoneToday] = useState(false);

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
      case TOPICS.TIMER_TODAY:
        setTimerWorking(message === 'Today_ON');
        break;
      case TOPICS.TIMER_DONE:
        setTimerDoneToday(message === 'Done_ON');
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
    if (data.timerWorking !== undefined) setTimerWorking(data.timerWorking);
    if (data.timerDoneToday !== undefined) setTimerDoneToday(data.timerDoneToday);
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
    
    const message = enabled ? `${day.charAt(0).toUpperCase() + day.slice(1)}_ON` : `${day.charAt(0).toUpperCase() + day.slice(1)}_OFF`;
    mqttService.publish(topicMap[day], message, true);
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

      <Container maxWidth="xl" sx={{ mt: 2, mb: 4 }}>
        
        {/* Status Bar */}
        <StatusIndicator status={pumpStatus} />
        
        {/* Timer Status Notice */}
        <TimerStatusIndicator 
          timerWorking={timerWorking} 
          timerDoneToday={timerDoneToday} 
        />

        {/* Main Layout */}
        <Box sx={{ mt: 3 }}>
          <Grid container spacing={3}>
            
            {/* Left: Tank Park */}
            <Grid item xs={12} lg={3}>
              <AnimatedWaterTank
                tankName="ถัง Park (ด้านบน)"
                ultrasonicPercent={ultrasonicPark}
                upStatus={parkUp}
                downStatus={parkDown}
              />
            </Grid>

            {/* Center: Tank Pub */}
            <Grid item xs={12} lg={3}>
              <AnimatedWaterTank
                tankName="ถัง Pub (ด้านล่าง)"
                ultrasonicPercent={ultrasonicPub}
                upStatus={pubUp}
                downStatus={pubDown}
              />
            </Grid>

            {/* Right Side: Controls + Timer */}
            <Grid item xs={12} lg={6}>
              <Stack spacing={3}>
                
                {/* Controls Row - 3 Columns */}
                <Grid container spacing={2}>
                  <Grid item xs={12} sm={4}>
                    <PumpControl 
                      pumpStatus={pumpStatus}
                      onToggle={handlePumpToggle}
                    />
                  </Grid>
                  <Grid item xs={12} sm={4}>
                    <AutoModeSwitch
                      autoMode={autoMode}
                      onToggle={handleAutoModeToggle}
                    />
                  </Grid>
                  <Grid item xs={12} sm={4}>
                    <TimerModeSwitch
                      timerMode={timerMode}
                      onToggle={handleTimerModeToggle}
                    />
                  </Grid>
                </Grid>

                {/* Timer Settings */}
                <TimerSettings 
                  startTime={timerStart}
                  stopTime={timerStop}
                  onTimeChange={handleTimeChange}
                />

                {/* Day Selector */}
                <DaySelector 
                  selectedDays={selectedDays}
                  onDayToggle={handleDayToggle}
                />

              </Stack>
            </Grid>

          </Grid>
        </Box>

      </Container>

      {/* Floating Dev Console */}
      <DummyDataController onDataChange={handleDummyDataChange} />
    </ThemeProvider>
  );
}

export default App;
