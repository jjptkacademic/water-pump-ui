export const getWaterGradient = (level) => {
  if (level > 70) {
    return 'linear-gradient(180deg, #66BB6A 0%, #2E7D32 100%)';
  }
  if (level > 30) {
    return 'linear-gradient(180deg, #42A5F5 0%, #1565C0 100%)';
  }
  return 'linear-gradient(180deg, #FF9800 0%, #E65100 100%)';
};

export const getStatusColor = (level) => {
  if (level > 70) return 'success';
  if (level > 30) return 'primary';
  return 'warning';
};

export const getSensorStatusColor = (status) => {
  switch (status) {
    case 'synced':
      return 'success';
    case 'fallback-digital':
      return 'info';
    case 'mismatch-warning':
      return 'warning';
    default:
      return 'default';
  }
};

export const getSensorStatusMessage = (status) => {
  switch (status) {
    case 'synced':
      return '✅ Sensors synchronized';
    case 'fallback-digital':
      return 'ℹ️ Using digital sensors';
    case 'mismatch-warning':
      return '⚠️ Sensor mismatch';
    default:
      return '🔄 Syncing...';
  }
};
