#!/usr/bin/env python3
"""
JPEG Serial Receiver — 接收 STM32 通过串口发来的 JPEG 图片

MCU 端协议: [4 字节长度 (uint32 LE)] + [JPEG 数据]

用法:
    python jpeg_receiver.py COM3 921600
"""

import sys
import os
import struct
import signal
from datetime import datetime

try:
    import serial
except ImportError:
    print("请先安装 pyserial: pip install pyserial")
    sys.exit(1)

# ====== 默认参数 ======
DEFAULT_PORT = "COM3"
DEFAULT_BAUD = 921600
OUTPUT_DIR   = "jpeg_captures"

running = True


def signal_handler(sig, frame):
    global running
    print("\n[!] 收到中断信号, 正在退出...")
    running = False


def sync_to_header(ser):
    """滑窗读取直到找到有效 JPEG 长度头"""
    buf = b""
    while running:
        byte = ser.read(1)
        if not byte:
            continue
        buf += byte
        if len(buf) >= 4:
            length = struct.unpack("<I", buf[-4:])[0]
            # 合理范围: 1KB ~ 500KB
            if 1024 < length < 512 * 1024:
                print(f"[*] 同步帧头, JPEG 长度 = {length} bytes ({length/1024:.1f} KB)")
                return length
            buf = buf[-3:]  # 滑窗
    return None


def main():
    global running

    port = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_PORT
    baud = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_BAUD

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    signal.signal(signal.SIGINT, signal_handler)

    print(f"[*] 打开 {port} @ {baud} bps ...")
    ser = serial.Serial(port, baud, timeout=1.0)
    ser.reset_input_buffer()
    print(f"[*] 等待数据 (Ctrl+C 退出) ...\n")

    frame_cnt = 0

    try:
        while running:
            # 1. 同步帧头
            jpeg_len = sync_to_header(ser)
            if jpeg_len is None:
                break

            # 2. 读取 JPEG 数据
            jpeg_data = bytearray()
            remaining = jpeg_len
            while remaining > 0 and running:
                chunk = ser.read(min(remaining, 4096))
                if not chunk:
                    continue
                jpeg_data.extend(chunk)
                remaining -= len(chunk)

            if not running:
                break

            if len(jpeg_data) < jpeg_len:
                print(f"    [WARN] 超时, 读到 {len(jpeg_data)}/{jpeg_len} bytes")
                continue

            # 3. 验证 JPEG 头
            if jpeg_data[0] != 0xFF or jpeg_data[1] != 0xD8:
                print(f"    [WARN] 非 JPEG (缺 FFD8), 丢弃")
                continue

            # 4. 保存
            frame_cnt += 1
            ts = datetime.now().strftime("%Y%m%d_%H%M%S")
            fname = os.path.join(OUTPUT_DIR, f"capture_{frame_cnt:04d}_{ts}.jpg")
            with open(fname, "wb") as f:
                f.write(jpeg_data)

            print(f"    [OK] {fname}  ({len(jpeg_data)/1024:.1f} KB)\n")

    except serial.SerialException as e:
        print(f"\n[!] 串口错误: {e}")
    finally:
        ser.close()
        print(f"\n[*] 完成, 共接收 {frame_cnt} 张, 保存在 '{OUTPUT_DIR}/'")

if __name__ == "__main__":
    main()
