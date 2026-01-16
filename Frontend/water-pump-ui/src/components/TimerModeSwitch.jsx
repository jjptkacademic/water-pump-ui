import { Card, CardContent, Switch, FormControlLabel, Typography } from '@mui/material';
import { Schedule } from '@mui/icons-material';

const TimerModeSwitch = ({ timerMode, onToggle }) => {
  return (
    <Card sx={{ height: '100%' }}>
      <CardContent>
        <Typography variant="h6" gutterBottom align="center" sx={{ display: 'flex', alignItems: 'center', justifyContent: 'center', gap: 1 }}>
          <Schedule /> โหมดตั้งเวลา
        </Typography>
        <FormControlLabel
          sx={{ 
            mt: 2,
            display: 'flex',
            justifyContent: 'center'
          }}
          control={
            <Switch
              checked={timerMode}
              onChange={(e) => onToggle(e.target.checked)}
              color="primary"
              size="medium"
            />
          }
          label={timerMode ? 'เปิดใช้งาน' : 'ปิดใช้งาน'}
        />
      </CardContent>
    </Card>
  );
};

export default TimerModeSwitch;
