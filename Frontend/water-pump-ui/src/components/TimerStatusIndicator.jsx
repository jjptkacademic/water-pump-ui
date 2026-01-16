import { Alert, Box, Typography } from '@mui/material';
import { CheckCircle, Schedule, Block } from '@mui/icons-material';

const TimerStatusIndicator = ({ timerWorking, timerDoneToday }) => {
  if (!timerWorking && !timerDoneToday) {
    return null; // ไม่แสดงอะไรถ้าไม่เกี่ยวกับ timer
  }

  return (
    <Box sx={{ mb: 2 }}>
      {timerWorking && !timerDoneToday && (
        <Alert severity="info" icon={<Schedule />}>
          <Typography variant="body2">
            📅 วันนี้มีกำหนดการตั้งเวลา - รอถึงเวลาทำงาน
          </Typography>
        </Alert>
      )}
      
      {timerDoneToday && (
        <Alert severity="success" icon={<CheckCircle />}>
          <Typography variant="body2">
            ✅ Timer ทำงานเสร็จแล้ววันนี้ - จะทำงานอีกครั้งในวันพรุ่งนี้
          </Typography>
        </Alert>
      )}
    </Box>
  );
};

export default TimerStatusIndicator;
