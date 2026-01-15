import { Box, Typography, Alert } from '@mui/material';
import { Power, PowerOff } from '@mui/icons-material';

const StatusIndicator = ({ status }) => {
  return (
    <Alert
      severity={status ? 'success' : 'info'}
      icon={status ? <Power /> : <PowerOff />}
      sx={{ mb: 2 }}
    >
      <Typography variant="h6">
        {status ? '🟢 ปั๊มกำลังทำงาน' : '⚫ ปั๊มหยุดทำงาน'}
      </Typography>
    </Alert>
  );
};

export default StatusIndicator;
