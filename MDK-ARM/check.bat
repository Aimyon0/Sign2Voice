@echo off
cd /d "e:\base\finaltest\MDK-ARM\HARDWARE\TIMER"
echo Checking file...
findstr /n "lcd_dma_done_flag\|dma_chunk_count\|removed" OV_Frame.c
