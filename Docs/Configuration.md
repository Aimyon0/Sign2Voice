# Configuration Management

## SystemConfig

```c
typedef struct {
    uint8_t volume;       // 0-30     speaker volume
    uint8_t threshold;    // 50-95    confidence threshold (%)
    uint8_t window_size;  // 3/5/7    sliding-window frame count
    uint8_t version;      // format version (currently 1)
    uint8_t _reserved[28];// padding to 32 bytes
} SystemConfig;
```

## Defaults

| Parameter | Default | Range |
|---|---|---|
| volume | 20 | 0-30 |
| threshold | 70 | 50-95, step 5 |
| window_size | 5 | 3, 5, 7 |

## Flash Storage

### Hardware

- **MCU**: STM32H743VIT6 (Dual-Bank, 2MB Flash)
- **Sector**: Bank 2 Sector 7 (0x081E0000-0x081FFFFF, 128KB)
- **Usage**: First 32 bytes only
- **Scatter file**: `ER_IROM1 0x08000000 0x001E0000` — reserves Sector 7 from linker

### API

```c
ErrorCode config_init(void);   // Load from Flash or use defaults
ErrorCode config_load(void);   // Read from Flash
ErrorCode config_save(void);   // Write to Flash (IRQs disabled, ~1s lag)
void      config_reset(void);  // Factory defaults (RAM only)
uint8_t   config_get_volume(void);
uint8_t   config_get_threshold(void);
uint8_t   config_get_window_size(void);
void      config_set_volume(uint8_t v);
void      config_set_threshold(uint8_t t);
void      config_set_window_size(uint8_t w);
```

### Write Sequence

```
config_save() → flash_storage_write()  [runs from SRAM (.RAM_D2)]
  ├── memcpy to 32-byte-aligned SRAM buffer
  ├── SCB_CleanDCache          // flush buffer to physical RAM
  ├── __disable_irq()
  ├── HAL_FLASH_Unlock()
  ├── HAL_FLASHEx_Erase()      // erase Bank 2 Sector 7
  ├── HAL_FLASH_Program() ×1   // program 32-byte flashword
  ├── HAL_FLASH_Lock()
  └── __enable_irq()
```

### Read Sequence

```
config_load() → flash_storage_read()
  └── memcpy(dst, 0x081E0000, 32)   // direct memory-mapped read
```

### When is config saved?

Only on explicit user action: **long-press KEY2** to exit Volume, Threshold, or Window Size settings pages. Not saved on every key press (avoids excessive flash wear).

### Debugging History (v2.1 Flash Persistence)

Flash persistence was the most challenging v2.1 feature. Issues encountered:

1. **Bank mismatch**: STM32H743 is DUAL-BANK. Address 0x081E0000 = Bank 2 Sector 7. Code initially used `FLASH_BANK_1` for erase, targeting wrong bank (0x080E0000) → erased code, caused HardFault.

2. **Scatter file overlap**: `ER_IROM1` originally spanned full 2MB. Linker could place RO data in Sector 7. After erase, code corruption → HardFault. Fixed by limiting to `0x001E0000`.

3. **SRAM execution**: Flash erase/program on H7 stalls instruction fetch from flash. `flash_storage_write()` must run from SRAM via `__attribute__((section(".RAM_D2")))`.

4. **Data corruption**: Original write passed `(uint32_t)(buf + i*32)` where `buf` pointer arithmetic could be ambiguous. Fixed to `(uint32_t)&buf[i*32]`.

5. **Cache**: `SCB_CleanDCache_by_Addr` required before flash write to ensure data flushed from D-Cache to physical SRAM.

6. **Programming delay**: `__HAL_FLASH_SET_PROGRAM_DELAY(FLASH_BANK_2)` needed for reliable programming at 400MHz HCLK.
