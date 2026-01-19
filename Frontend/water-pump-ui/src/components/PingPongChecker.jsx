import { useState } from 'react';
import { Paper, Typography, Button, Box, Chip, Stack } from '@mui/material';
import SignalCellularAltIcon from '@mui/icons-material/SignalCellularAlt';
import MemoryIcon from '@mui/icons-material/Memory';
import AccessTimeIcon from '@mui/icons-material/AccessTime';
import WifiIcon from '@mui/icons-material/Wifi';

function PingPongChecker({ onPing, pongData }) {
  const [checking, setChecking] = useState(false);

  const handlePing = () => {
    setChecking(true);
    onPing();
    // Reset checking state after 3 seconds
    setTimeout(() => setChecking(false), 3000);
  };

  // Parse pong response: "Uptime:0h2m27s|Heap:16KB|WiFi:-65dBm"
  const parsePong = (data) => {
    if (!data) return null;

    const parts = data.split('|');
    const result = {};

    parts.forEach(part => {
      const [key, value] = part.split(':');
      result[key] = value;
    });

    return result;
  };

  const parsed = parsePong(pongData);

  // Get WiFi signal quality
  const getWifiQuality = (rssi) => {
    if (!rssi) return { color: 'default', text: 'N/A' };
    const value = parseInt(rssi);
    if (value >= -60) return { color: 'success', text: 'ดีมาก' };
    if (value >= -70) return { color: 'warning', text: 'พอใช้' };
    return { color: 'error', text: 'อ่อน' };
  };

  const wifiQuality = parsed ? getWifiQuality(parsed.WiFi) : { color: 'default', text: 'N/A' };

  return (
    <Paper
      elevation={3}
      sx={{
        p: 3,
        background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
        color: 'white'
      }}
    >
      <Typography variant="h6" gutterBottom sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
        <SignalCellularAltIcon />
        เช็คสถานะ ESP8266
      </Typography>

      {parsed ? (
        <Stack spacing={2} sx={{ mt: 2 }}>
          {/* Uptime */}
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
            <AccessTimeIcon fontSize="small" />
            <Typography variant="body2">
              <strong>เปิดมา:</strong> {parsed.Uptime}
            </Typography>
          </Box>

          {/* Heap */}
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
            <MemoryIcon fontSize="small" />
            <Typography variant="body2">
              <strong>RAM เหลือ:</strong> {parsed.Heap}
            </Typography>
          </Box>

          {/* WiFi Signal */}
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
            <WifiIcon fontSize="small" />
            <Typography variant="body2" sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
              <strong>WiFi:</strong> {parsed.WiFi}
              <Chip
                label={wifiQuality.text}
                color={wifiQuality.color}
                size="small"
                sx={{ height: 20 }}
              />
            </Typography>
          </Box>
        </Stack>
      ) : (
        <Typography variant="body2" sx={{ mt: 2, opacity: 0.8 }}>
          กดปุ่มด้านล่างเพื่อเช็คสถานะ
        </Typography>
      )}

      <Button
        fullWidth
        variant="contained"
        onClick={handlePing}
        disabled={checking}
        sx={{
          mt: 3,
          bgcolor: 'rgba(255, 255, 255, 0.2)',
          '&:hover': {
            bgcolor: 'rgba(255, 255, 255, 0.3)'
          }
        }}
      >
        {checking ? 'กำลังเช็ค...' : '🔍 เช็คการเชื่อมต่อ'}
      </Button>
    </Paper>
  );
}

export default PingPongChecker;
