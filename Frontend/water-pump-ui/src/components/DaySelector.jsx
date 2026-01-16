import { Card, CardContent, Typography, Stack, Chip } from '@mui/material';
import { CalendarMonth } from '@mui/icons-material';

const DAYS = [
  { label: 'จ', value: 'mon', full: 'จันทร์' },
  { label: 'อ', value: 'tue', full: 'อังคาร' },
  { label: 'พ', value: 'wed', full: 'พุธ' },
  { label: 'พฤ', value: 'thu', full: 'พฤหัสบดี' },
  { label: 'ศ', value: 'fri', full: 'ศุกร์' },
  { label: 'ส', value: 'sat', full: 'เสาร์' },
  { label: 'อา', value: 'sun', full: 'อาทิตย์' }
];

const DaySelector = ({ selectedDays = {}, onDayToggle }) => {
  const handleClick = (day) => {
    if (onDayToggle) {
      onDayToggle(day, !selectedDays[day]);
    }
  };

  return (
    <Card>
      <CardContent>
        <Typography variant="h6" gutterBottom sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
          <CalendarMonth /> เลือกวันทำงาน
        </Typography>
        <Stack direction="row" spacing={1} sx={{ mt: 2, flexWrap: 'wrap', gap: 1 }}>
          {DAYS.map((day) => (
            <Chip
              key={day.value}
              label={day.label}
              color={selectedDays[day.value] ? 'primary' : 'default'}
              variant={selectedDays[day.value] ? 'filled' : 'outlined'}
              onClick={() => handleClick(day.value)}
              sx={{ 
                minWidth: 50,
                fontSize: '1rem',
                fontWeight: selectedDays[day.value] ? 'bold' : 'normal'
              }}
            />
          ))}
        </Stack>
      </CardContent>
    </Card>
  );
};

export default DaySelector;
