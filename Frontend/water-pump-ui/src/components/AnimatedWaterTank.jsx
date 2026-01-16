import { Box, Typography, Chip, Alert } from '@mui/material';
import { ArrowUpward, Circle, ArrowDownward } from '@mui/icons-material';
import { useWaterLevel } from '../hooks/useWaterLevel';
import { useDualSensorSync } from '../hooks/useDualSensorSync';
import { getWaterGradient, getSensorStatusColor, getSensorStatusMessage } from '../utils/waterColors';
import '../styles/WaterTank.css';

const AnimatedWaterTank = ({ 
  tankName,
  ultrasonicPercent,
  upStatus,
  downStatus 
}) => {
  const { displayLevel, sensorStatus } = useDualSensorSync(
    ultrasonicPercent,
    { upStatus, downStatus }
  );

  const { animatedLevel, isChanging } = useWaterLevel(displayLevel);

  const waveSpeed = isChanging ? '3s' : '8s';

  return (
    <Box className="tank-container">
      <Typography variant="h6" gutterBottom align="center" sx={{ mb: 3 }}>
        {tankName}
      </Typography>

      <Box className="tank-shell">
        
        <Box className="sensor-markers">
          <Box className={`marker full ${upStatus ? 'active' : ''}`}>
            <ArrowUpward fontSize="small" />
            <span>Full</span>
          </Box>
          <Box className="marker half">
            <Circle fontSize="small" />
            <span>Half</span>
          </Box>
          <Box className={`marker low ${downStatus ? 'active' : ''}`}>
            <ArrowDownward fontSize="small" />
            <span>Low</span>
          </Box>
        </Box>

        <Box
          className="water"
          sx={{
            height: `${displayLevel}%`,
            background: getWaterGradient(displayLevel),
            '--wave-speed': waveSpeed,
            transition: 'height 1.5s cubic-bezier(0.4, 0, 0.2, 1)'
          }}
        >
          <div className="wave wave1" />
          <div className="wave wave2" />
          <div className="wave wave3" />
        </Box>

        <Typography className="level-text" variant="h4">
          {displayLevel.toFixed(1)}%
        </Typography>
      </Box>

      <Alert 
        severity={getSensorStatusColor(sensorStatus)} 
        sx={{ mt: 2 }}
      >
        {getSensorStatusMessage(sensorStatus)}
      </Alert>
    </Box>
  );
};

export default AnimatedWaterTank;
