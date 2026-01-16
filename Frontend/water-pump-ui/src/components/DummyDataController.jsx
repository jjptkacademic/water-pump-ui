import { useState } from 'react';
import { 
  Box, 
  Card, 
  CardContent, 
  Typography, 
  Button, 
  Slider, 
  Stack,
  Switch,
  FormControlLabel,
  Divider,
  IconButton,
  Collapse
} from '@mui/material';
import { BugReport, Close, DragIndicator, Minimize, ExpandMore } from '@mui/icons-material';

const DummyDataController = ({ onDataChange }) => {
  const [isOpen, setIsOpen] = useState(false);
  const [isMinimized, setIsMinimized] = useState(false);
  const [useDummyData, setUseDummyData] = useState(false);
  const [parkLevel, setParkLevel] = useState(50);
  const [pubLevel, setPubLevel] = useState(75);
  const [pumpOn, setPumpOn] = useState(false);
  const [parkUpSensor, setParkUpSensor] = useState(0);
  const [parkDownSensor, setParkDownSensor] = useState(0);
  const [pubUpSensor, setPubUpSensor] = useState(1);
  const [pubDownSensor, setPubDownSensor] = useState(0);

  const [animating, setAnimating] = useState(false);
  
  // Timer Status
  const [timerWorking, setTimerWorking] = useState(false);
  const [timerDone, setTimerDone] = useState(false);

  const handleParkChange = (event, newValue) => {
    setParkLevel(newValue);
    
    if (newValue > 80) {
      setParkUpSensor(1);
      setParkDownSensor(0);
    } else if (newValue < 20) {
      setParkUpSensor(0);
      setParkDownSensor(1);
    } else {
      setParkUpSensor(0);
      setParkDownSensor(0);
    }

    if (useDummyData && onDataChange) {
      onDataChange({
        ultrasonicPark: newValue,
        parkUp: newValue > 80 ? 1 : 0,
        parkDown: newValue < 20 ? 1 : 0
      });
    }
  };

  const handlePubChange = (event, newValue) => {
    setPubLevel(newValue);
    
    if (newValue > 80) {
      setPubUpSensor(1);
      setPubDownSensor(0);
    } else if (newValue < 20) {
      setPubUpSensor(0);
      setPubDownSensor(1);
    } else {
      setPubUpSensor(0);
      setPubDownSensor(0);
    }

    if (useDummyData && onDataChange) {
      onDataChange({
        ultrasonicPub: newValue,
        pubUp: newValue > 80 ? 1 : 0,
        pubDown: newValue < 20 ? 1 : 0
      });
    }
  };

  const handlePumpToggle = () => {
    setPumpOn(!pumpOn);
    if (useDummyData && onDataChange) {
      onDataChange({
        pumpStatus: !pumpOn
      });
    }
  };

  const handleDummyToggle = (e) => {
    const enabled = e.target.checked;
    setUseDummyData(enabled);
    
    if (enabled && onDataChange) {
      onDataChange({
        ultrasonicPark: parkLevel,
        ultrasonicPub: pubLevel,
        parkUp: parkUpSensor,
        parkDown: parkDownSensor,
        pubUp: pubUpSensor,
        pubDown: pubDownSensor,
        pumpStatus: pumpOn,
        timerWorking: timerWorking,
        timerDoneToday: timerDone
      });
    }
  };

  const handleTimerWorkingToggle = () => {
    const newValue = !timerWorking;
    setTimerWorking(newValue);
    if (useDummyData && onDataChange) {
      onDataChange({ timerWorking: newValue });
    }
  };

  const handleTimerDoneToggle = () => {
    const newValue = !timerDone;
    setTimerDone(newValue);
    if (useDummyData && onDataChange) {
      onDataChange({ timerDoneToday: newValue });
    }
  };

  const simulatePumping = () => {
    if (animating) return;
    
    setAnimating(true);
    setPumpOn(true);
    
    if (onDataChange) {
      onDataChange({ pumpStatus: true });
    }

    let currentPark = parkLevel;
    let currentPub = pubLevel;

    const interval = setInterval(() => {
      currentPark = Math.min(100, currentPark + 2);
      currentPub = Math.max(0, currentPub - 3);

      setParkLevel(currentPark);
      setPubLevel(currentPub);

      if (useDummyData && onDataChange) {
        onDataChange({
          ultrasonicPark: currentPark,
          ultrasonicPub: currentPub,
          parkUp: currentPark > 80 ? 1 : 0,
          parkDown: currentPark < 20 ? 1 : 0,
          pubUp: currentPub > 80 ? 1 : 0,
          pubDown: currentPub < 20 ? 1 : 0
        });
      }

      if (currentPark >= 100 || currentPub <= 0) {
        clearInterval(interval);
        setPumpOn(false);
        setAnimating(false);
        if (onDataChange) {
          onDataChange({ pumpStatus: false });
        }
      }
    }, 200);
  };

  const resetLevels = () => {
    setParkLevel(0);
    setPubLevel(100);
    setParkUpSensor(0);
    setParkDownSensor(1);
    setPubUpSensor(1);
    setPubDownSensor(0);
    setPumpOn(false);

    if (useDummyData && onDataChange) {
      onDataChange({
        ultrasonicPark: 0,
        ultrasonicPub: 100,
        parkUp: 0,
        parkDown: 1,
        pubUp: 1,
        pubDown: 0,
        pumpStatus: false
      });
    }
  };

  if (!isOpen) {
    return (
      <Box
        sx={{
          position: 'fixed',
          bottom: 20,
          right: 20,
          zIndex: 1300
        }}
      >
        <Button
          variant="contained"
          color="warning"
          startIcon={<BugReport />}
          onClick={() => setIsOpen(true)}
          sx={{
            boxShadow: 4,
            '&:hover': {
              boxShadow: 8
            }
          }}
        >
          🎮 Dev Console
        </Button>
      </Box>
    );
  }

  return (
    <Box
      sx={{
        position: 'fixed',
        bottom: 20,
        right: 20,
        width: 400,
        maxHeight: '80vh',
        zIndex: 1300,
        boxShadow: 8
      }}
    >
      <Card sx={{ 
        backgroundColor: 'rgba(18, 18, 18, 0.98)', 
        border: '2px solid #FF9800',
        backdropFilter: 'blur(10px)'
      }}>
        {/* Header */}
        <Box
          sx={{
            display: 'flex',
            alignItems: 'center',
            p: 1,
            backgroundColor: 'rgba(255, 152, 0, 0.2)',
            cursor: 'move',
            borderBottom: '1px solid #FF9800'
          }}
        >
          <DragIndicator sx={{ mr: 1, color: 'warning.main' }} />
          <BugReport sx={{ mr: 1, color: 'warning.main' }} />
          <Typography variant="subtitle1" sx={{ flexGrow: 1, fontWeight: 'bold' }}>
            Dev Console
          </Typography>
          <IconButton size="small" onClick={() => setIsMinimized(!isMinimized)}>
            {isMinimized ? <ExpandMore /> : <Minimize />}
          </IconButton>
          <IconButton size="small" onClick={() => setIsOpen(false)}>
            <Close />
          </IconButton>
        </Box>

        {/* Content */}
        <Collapse in={!isMinimized}>
          <CardContent sx={{ maxHeight: 'calc(80vh - 60px)', overflowY: 'auto' }}>
            <FormControlLabel
              control={
                <Switch
                  checked={useDummyData}
                  onChange={handleDummyToggle}
                  color="warning"
                  size="small"
                />
              }
              label={
                <Typography variant="body2">
                  {useDummyData ? '🟢 Dummy Mode' : '⚪ MQTT Mode'}
                </Typography>
              }
              sx={{ mb: 2 }}
            />

            <Divider sx={{ my: 2 }} />

            <Stack spacing={2.5}>
              <Box>
                <Typography variant="body2" gutterBottom>
                  🚰 Park: {parkLevel}% 
                  {parkUpSensor ? ' [▲]' : ''} 
                  {parkDownSensor ? ' [▼]' : ''}
                </Typography>
                <Slider
                  value={parkLevel}
                  onChange={handleParkChange}
                  min={0}
                  max={100}
                  size="small"
                  disabled={!useDummyData}
                  sx={{ mt: 1 }}
                />
              </Box>

              <Box>
                <Typography variant="body2" gutterBottom>
                  💧 Pub: {pubLevel}%
                  {pubUpSensor ? ' [▲]' : ''} 
                  {pubDownSensor ? ' [▼]' : ''}
                </Typography>
                <Slider
                  value={pubLevel}
                  onChange={handlePubChange}
                  min={0}
                  max={100}
                  size="small"
                  disabled={!useDummyData}
                  sx={{ mt: 1 }}
                />
              </Box>

              <Divider />

              <Button
                variant="contained"
                color={pumpOn ? 'success' : 'default'}
                size="small"
                onClick={handlePumpToggle}
                disabled={!useDummyData}
                fullWidth
              >
                {pumpOn ? '⚡ ON' : '⚪ OFF'}
              </Button>

              <Divider />

              {/* Timer Status Controls */}
              <Typography variant="caption" sx={{ color: 'warning.main' }}>
                ⏰ Timer Status
              </Typography>

              <Stack direction="row" spacing={1}>
                <Button
                  variant={timerWorking ? 'contained' : 'outlined'}
                  color="info"
                  size="small"
                  onClick={handleTimerWorkingToggle}
                  disabled={!useDummyData}
                  fullWidth
                >
                  📅 Working
                </Button>
                <Button
                  variant={timerDone ? 'contained' : 'outlined'}
                  color="success"
                  size="small"
                  onClick={handleTimerDoneToggle}
                  disabled={!useDummyData}
                  fullWidth
                >
                  ✅ Done
                </Button>
              </Stack>

              <Divider />

              <Stack direction="row" spacing={1}>
                <Button
                  variant="outlined"
                  color="primary"
                  size="small"
                  onClick={simulatePumping}
                  disabled={!useDummyData || animating}
                  fullWidth
                >
                  {animating ? '🔄' : '▶️'}
                </Button>
                <Button
                  variant="outlined"
                  color="secondary"
                  size="small"
                  onClick={resetLevels}
                  disabled={!useDummyData || animating}
                  fullWidth
                >
                  🔄
                </Button>
              </Stack>
            </Stack>

            <Typography variant="caption" sx={{ display: 'block', mt: 2, color: 'warning.main' }}>
              💡 Toggle switch to use Dummy Data
            </Typography>
          </CardContent>
        </Collapse>
      </Card>
    </Box>
  );
};

export default DummyDataController;
