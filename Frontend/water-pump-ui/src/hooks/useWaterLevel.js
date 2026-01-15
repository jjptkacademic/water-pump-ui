import { useState, useEffect } from 'react';
import { useSpring } from '@react-spring/web';

export const useWaterLevel = (mqttLevel) => {
  const [currentLevel, setCurrentLevel] = useState(0);
  const [previousLevel, setPreviousLevel] = useState(0);

  const waterAnimation = useSpring({
    from: { height: previousLevel },
    to: { height: currentLevel },
    config: {
      tension: 50,
      friction: 14,
      mass: 1
    }
  });

  useEffect(() => {
    if (mqttLevel !== currentLevel && mqttLevel !== null && mqttLevel !== undefined) {
      setPreviousLevel(currentLevel);
      setCurrentLevel(mqttLevel);
    }
  }, [mqttLevel]);

  return {
    animatedLevel: waterAnimation.height,
    currentLevel,
    previousLevel,
    isChanging: currentLevel !== previousLevel
  };
};
