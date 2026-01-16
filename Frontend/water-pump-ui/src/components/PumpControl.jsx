import { Card, CardContent, Button, Stack, Typography } from '@mui/material';
import { PowerSettingsNew, Block } from '@mui/icons-material';

const PumpControl = ({ pumpStatus, onToggle }) => {
  return (
    <Card sx={{ height: '100%' }}>
      <CardContent>
        <Typography variant="h6" gutterBottom align="center">
          🎛️ ควบคุมปั๊มด้วยมือ
        </Typography>
        <Stack direction="row" spacing={2} sx={{ mt: 3 }}>
          <Button
            variant="contained"
            color="success"
            size="large"
            startIcon={<PowerSettingsNew />}
            onClick={() => onToggle(true)}
            disabled={pumpStatus}
            fullWidth
          >
            เปิดปั๊ม
          </Button>
          <Button
            variant="contained"
            color="error"
            size="large"
            startIcon={<Block />}
            onClick={() => onToggle(false)}
            disabled={!pumpStatus}
            fullWidth
          >
            ปิดปั๊ม
          </Button>
        </Stack>
      </CardContent>
    </Card>
  );
};

export default PumpControl;
