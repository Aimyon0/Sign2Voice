import re

with open(r'e:\base\finaltest\MDK-ARM\HARDWARE\TIMER\OV_Frame.c', 'rb') as f:
    content = f.read()

# Find the callback function using bytes pattern
old = b'''    if(hspi->Instance == SPI2)
    {
        dma_chunk_count++;'''

new = b'''    if(hspi->Instance == SPI2)
    {
        lcd_dma_done_flag = 1;'''

# Find the position
pos = content.find(old)
if pos == -1:
    print("Pattern not found!")
    # Try searching for just dma_chunk_count
    pos2 = content.find(b'dma_chunk_count++;')
    if pos2 >= 0:
        print(f"Found 'dma_chunk_count++;' at byte offset {pos2}")
        # Show surrounding bytes
        print("Surrounding:", repr(content[pos2-50:pos2+150]))
else:
    print(f"Found at byte offset {pos}")
    # Find the end of the block (after dma_chunk_count = 0; })
    end_pattern = b'dma_chunk_count = 0;'
    end_pos = content.find(end_pattern, pos)
    if end_pos >= 0:
        # Find closing braces after end_pos
        close_pos = content.find(b'    }\n}', end_pos)
        if close_pos >= 0:
            close_pos = content.find(b'\n', close_pos) + 1  # Include newline
        else:
            # Try another pattern
            close_pos = content.find(b'\n    }\n}', end_pos)
            if close_pos >= 0:
                close_pos += len(b'\n    }\n}')

        print(f"End position after old block: {close_pos}")
        print(f"Old block bytes: {repr(content[pos:close_pos])}")
