import { Card, CardContent, Button, Stack, Typography } from '@mui/material';
import { PowerSettingsNew, Block } from '@mui/icons-material';

const PumpControl = ({ pumpStatus, onToggle }) => {
  return (
    <Card>
      <CardContent>
        <Typography variant="h6" gutterBottom>
          🎛️ ควบคุมปั๊มด้วยมือ
        </Typography>
        <Stack direction="row" spacing={2} sx={{ mt: 2 }}>
          <Button
            variant="contained"
            color="success"
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
