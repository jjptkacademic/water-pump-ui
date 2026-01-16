import { Card, CardContent, Switch, FormControlLabel, Typography } from '@mui/material';

const AutoModeSwitch = ({ autoMode, onToggle }) => {
  return (
    <Card sx={{ height: '100%' }}>
      <CardContent>
        <Typography variant="h6" gutterBottom align="center">
          🤖 โหมดอัตโนมัติ
        </Typography>
        <FormControlLabel
          sx={{ 
            mt: 2,
            display: 'flex',
            justifyContent: 'center'
          }}
          control={
            <Switch
              checked={autoMode}
              onChange={(e) => onToggle(e.target.checked)}
              color="secondary"
              size="medium"
            />
          }
          label={autoMode ? 'เปิดใช้งาน' : 'ปิดใช้งาน'}
        />
      </CardContent>
    </Card>
  );
};

export default AutoModeSwitch;
