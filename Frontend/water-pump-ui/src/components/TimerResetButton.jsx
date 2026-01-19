import { useState } from 'react';
import { Paper, Button, Typography, Box } from '@mui/material';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';

function TimerResetButton({ onReset, timerExecutedToday }) {
  const [resetting, setResetting] = useState(false);

  const handleReset = () => {
    setResetting(true);
    onReset();
    setTimeout(() => setResetting(false), 2000);
  };

  return (
    <Paper
      elevation={3}
      sx={{
        p: 3,
        background: timerExecutedToday
          ? 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)'
          : 'linear-gradient(135deg, #f093fb 0%, #f5576c 100%)',
        color: 'white',
        textAlign: 'center'
      }}
    >
      <Typography variant="h6" gutterBottom sx={{ display: 'flex', alignItems: 'center', justifyContent: 'center', gap: 1 }}>
        <RestartAltIcon />
        RESET TIMER
      </Typography>

      <Box sx={{ my: 2 }}>
        {timerExecutedToday ? (
          <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'center', gap: 1 }}>
            <CheckCircleIcon />
            <Typography variant="body2">
              ✅ Timer ทำงานไปแล้ววันนี้
            </Typography>
          </Box>
        ) : (
          <Typography variant="body2" sx={{ opacity: 0.9 }}>
            ⏳ Timer ยังไม่ทำงานวันนี้
          </Typography>
        )}
      </Box>

      <Button
        fullWidth
        variant="contained"
        onClick={handleReset}
        disabled={resetting}
        startIcon={<RestartAltIcon />}
        sx={{
          bgcolor: 'rgba(255, 255, 255, 0.2)',
          '&:hover': {
            bgcolor: 'rgba(255, 255, 255, 0.3)'
          },
          '&:disabled': {
            bgcolor: 'rgba(255, 255, 255, 0.1)',
            color: 'rgba(255, 255, 255, 0.5)'
          }
        }}
      >
        {resetting ? 'กำลัง Reset...' : 'Reset Timer Flag'}
      </Button>

      <Typography variant="caption" sx={{ mt: 2, display: 'block', opacity: 0.8 }}>
        ใช้เพื่อให้ Timer ทำงานซ้ำในวันเดียวกัน
      </Typography>
    </Paper>
  );
}

export default TimerResetButton;
