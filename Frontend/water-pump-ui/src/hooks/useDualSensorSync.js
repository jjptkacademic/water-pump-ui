import { useState, useEffect } from 'react';

export const useDualSensorSync = (ultrasonicPercent, digitalSensors) => {
  const [displayLevel, setDisplayLevel] = useState(0);
  const [sensorStatus, setSensorStatus] = useState('syncing');

  useEffect(() => {
    const { upStatus, downStatus } = digitalSensors;
    let level = ultrasonicPercent;

    if (ultrasonicPercent === null || ultrasonicPercent === undefined || ultrasonicPercent < 0) {
      if (upStatus === 1) {
        level = 100;
      } else if (upStatus === 0 && downStatus === 0) {
        level = 50;
      } else if (downStatus === 1) {
        level = 0;
      }
      setSensorStatus('fallback-digital');
    }
    else if (
      (ultrasonicPercent > 80 && upStatus === 0) ||
      (ultrasonicPercent < 20 && downStatus === 0)
    ) {
      setSensorStatus('mismatch-warning');
    }
    else {
      setSensorStatus('synced');
    }

    setDisplayLevel(level);
  }, [ultrasonicPercent, digitalSensors]);

  return { displayLevel, sensorStatus };
};
