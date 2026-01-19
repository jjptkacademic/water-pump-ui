import { useState } from 'react';
import { Paper, Typography, Slider, Box, Grid, Chip } from '@mui/material';
import OpacityIcon from '@mui/icons-material/Opacity';
import ArrowUpwardIcon from '@mui/icons-material/ArrowUpward';
import ArrowDownwardIcon from '@mui/icons-material/ArrowDownward';

function ThresholdSliders({ onThresholdChange }) {
  const [parkStart, setParkStart] = useState(30);
  const [parkStop, setParkStop] = useState(90);
  const [pubMin, setPubMin] = useState(20);

  const handleParkStartChange = (event, newValue) => {
    setParkStart(newValue);
  };

  const handleParkStartCommit = (event, newValue) => {
    onThresholdChange('park-start', newValue);
  };

  const handleParkStopChange = (event, newValue) => {
    setParkStop(newValue);
  };

  const handleParkStopCommit = (event, newValue) => {
    onThresholdChange('park-stop', newValue);
  };

  const handlePubMinChange = (event, newValue) => {
    setPubMin(newValue);
  };

  const handlePubMinCommit = (event, newValue) => {
    onThresholdChange('pub-min', newValue);
  };

  return (
    <Paper
      elevation={3}
      sx={{
        p: 3,
        background: 'linear-gradient(135deg, #fa709a 0%, #fee140 100%)'
      }}
    >
      <Typography variant="h6" gutterBottom sx={{ display: 'flex', alignItems: 'center', gap: 1, color: '#fff' }}>
        <OpacityIcon />
        ตั้งค่า Threshold
      </Typography>

      <Grid container spacing={3} sx={{ mt: 1 }}>
        {/* Park Start */}
        <Grid item xs={12} md={4}>
          <Box>
            <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', mb: 1 }}>
              <Typography variant="body2" sx={{ color: '#fff', fontWeight: 'bold' }}>
                <ArrowDownwardIcon fontSize="small" sx={{ verticalAlign: 'middle' }} /> เปิดปั๊ม (สวน)
              </Typography>
              <Chip
                label={`${parkStart}%`}
                size="small"
                sx={{ bgcolor: 'rgba(255,255,255,0.3)', color: '#fff', fontWeight: 'bold' }}
              />
            </Box>
            <Slider
              value={parkStart}
              onChange={handleParkStartChange}
              onChangeCommitted={handleParkStartCommit}
              min={0}
              max={100}
              valueLabelDisplay="auto"
              valueLabelFormat={(value) => `${value}%`}
              sx={{
                color: '#fff',
                '& .MuiSlider-thumb': {
                  bgcolor: '#fff'
                },
                '& .MuiSlider-track': {
                  bgcolor: '#fff'
                },
                '& .MuiSlider-rail': {
                  bgcolor: 'rgba(255,255,255,0.3)'
                }
              }}
            />
            <Typography variant="caption" sx={{ color: 'rgba(255,255,255,0.8)' }}>
              เมื่อน้ำสวน {'<'} {parkStart}%
            </Typography>
          </Box>
        </Grid>

        {/* Park Stop */}
        <Grid item xs={12} md={4}>
          <Box>
            <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', mb: 1 }}>
              <Typography variant="body2" sx={{ color: '#fff', fontWeight: 'bold' }}>
                <ArrowUpwardIcon fontSize="small" sx={{ verticalAlign: 'middle' }} /> หยุดปั๊ม (สวน)
              </Typography>
              <Chip
                label={`${parkStop}%`}
                size="small"
                sx={{ bgcolor: 'rgba(255,255,255,0.3)', color: '#fff', fontWeight: 'bold' }}
              />
            </Box>
            <Slider
              value={parkStop}
              onChange={handleParkStopChange}
              onChangeCommitted={handleParkStopCommit}
              min={0}
              max={100}
              valueLabelDisplay="auto"
              valueLabelFormat={(value) => `${value}%`}
              sx={{
                color: '#fff',
                '& .MuiSlider-thumb': {
                  bgcolor: '#fff'
                },
                '& .MuiSlider-track': {
                  bgcolor: '#fff'
                },
                '& .MuiSlider-rail': {
                  bgcolor: 'rgba(255,255,255,0.3)'
                }
              }}
            />
            <Typography variant="caption" sx={{ color: 'rgba(255,255,255,0.8)' }}>
              เมื่อน้ำสวน {'>='} {parkStop}%
            </Typography>
          </Box>
        </Grid>

        {/* Pub Min */}
        <Grid item xs={12} md={4}>
          <Box>
            <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', mb: 1 }}>
              <Typography variant="body2" sx={{ color: '#fff', fontWeight: 'bold' }}>
                <OpacityIcon fontSize="small" sx={{ verticalAlign: 'middle' }} /> น้ำคลองขั้นต่ำ
              </Typography>
              <Chip
                label={`${pubMin}%`}
                size="small"
                sx={{ bgcolor: 'rgba(255,255,255,0.3)', color: '#fff', fontWeight: 'bold' }}
              />
            </Box>
            <Slider
              value={pubMin}
              onChange={handlePubMinChange}
              onChangeCommitted={handlePubMinCommit}
              min={0}
              max={100}
              valueLabelDisplay="auto"
              valueLabelFormat={(value) => `${value}%`}
              sx={{
                color: '#fff',
                '& .MuiSlider-thumb': {
                  bgcolor: '#fff'
                },
                '& .MuiSlider-track': {
                  bgcolor: '#fff'
                },
                '& .MuiSlider-rail': {
                  bgcolor: 'rgba(255,255,255,0.3)'
                }
              }}
            />
            <Typography variant="caption" sx={{ color: 'rgba(255,255,255,0.8)' }}>
              น้ำคลอง {'>='} {pubMin}%
            </Typography>
          </Box>
        </Grid>
      </Grid>
    </Paper>
  );
}

export default ThresholdSliders;
