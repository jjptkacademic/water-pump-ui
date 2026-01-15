import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  base: '/Arduino73/',
  build: {
    outDir: 'dist',
    sourcemap: false
  },
  server: {
    port: 5173,
    open: true
  }
})
