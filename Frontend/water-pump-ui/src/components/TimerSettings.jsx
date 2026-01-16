import { Card, CardContent, Typography, TextField, Stack, Box } from '@mui/material';
import { AccessTime } from '@mui/icons-material';

const TimerSettings = ({ startTime = '06:00', stopTime = '08:00', onTimeChange }) => {
  const handleStartChange = (e) => {
    if (onTimeChange) {
      onTimeChange('start', e.target.value);
    }
  };

  const handleStopChange = (e) => {
    if (onTimeChange) {
      onTimeChange('stop', e.target.value);
    }
  };

  return (
    <Card>
      <CardContent>
        <Typography variant="h6" gutterBottom sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
          <AccessTime /> ตั้งเวลาทำงาน
        </Typography>
        <Stack direction="row" spacing={2} sx={{ mt: 2 }}>
          <Box sx={{ flex: 1 }}>
            <TextField
              label="เวลาเริ่ม"
              type="time"
              value={startTime}
              onChange={handleStartChange}
              fullWidth
              InputLabelProps={{
                shrink: true,
              }}
              inputProps={{
                step: 300, // 5 min
              }}
            />
          </Box>
          <Box sx={{ flex: 1 }}>
            <TextField
              label="เวลาหยุด"
              type="time"
              value={stopTime}
              onChange={handleStopChange}
              fullWidth
              InputLabelProps={{
                shrink: true,
              }}
              inputProps={{
                step: 300,
              }}
            />
          </Box>
        </Stack>
      </CardContent>
    </Card>
  );
};

export default TimerSettings;
