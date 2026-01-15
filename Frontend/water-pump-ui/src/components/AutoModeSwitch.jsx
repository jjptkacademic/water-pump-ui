import { Card, CardContent, Switch, FormControlLabel, Typography } from '@mui/material';

const AutoModeSwitch = ({ autoMode, onToggle }) => {
  return (
    <Card>
      <CardContent>
        <Typography variant="h6" gutterBottom>
          🤖 โหมดอัตโนมัติ
        </Typography>
        <FormControlLabel
          control={
            <Switch
              checked={autoMode}
              onChange={(e) => onToggle(e.target.checked)}
              color="secondary"
            />
          }
          label={autoMode ? 'เปิดใช้งาน' : 'ปิดใช้งาน'}
        />
      </CardContent>
    </Card>
  );
};

export default AutoModeSwitch;
