import { Alert, Box, Typography } from '@mui/material';
import { CheckCircle } from '@mui/icons-material';

const TimerStatusIndicator = ({ timerExecutedToday }) => {
  if (!timerExecutedToday) {
    return null; // ไม่แสดงอะไรถ้า timer ยังไม่ทำงาน
  }

  return (
    <Box sx={{ mb: 2 }}>
      <Alert severity="success" icon={<CheckCircle />}>
        <Typography variant="body2">
          ✅ Timer ทำงานเสร็จแล้ววันนี้ - จะทำงานอีกครั้งในวันพรุ่งนี้
        </Typography>
      </Alert>
    </Box>
  );
};

export default TimerStatusIndicator;
