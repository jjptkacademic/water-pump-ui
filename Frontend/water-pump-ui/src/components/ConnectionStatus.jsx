import { Chip } from '@mui/material';
import { Wifi, WifiOff } from '@mui/icons-material';

const ConnectionStatus = ({ connected }) => {
  return (
    <Chip
      icon={connected ? <Wifi /> : <WifiOff />}
      label={connected ? 'Connected' : 'Disconnected'}
      color={connected ? 'success' : 'error'}
      variant="outlined"
    />
  );
};

export default ConnectionStatus;
