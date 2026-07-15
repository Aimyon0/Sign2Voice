/**
 ******************************************************************************
 * @file    mp3.c
 * @brief   MP3-TF-16P 模块驱动 (YX5200 芯片)
 *
 * @note    硬件连接:
 *           MCU PA9 (USART1_TX) -> MP3 模块 RX
 *           MCU PA10 (USART1_RX) -> MP3 模块 TX
 *
 *           协议参考: MP3-TF-16P 使用说明书 V1.6
 *           YX5200 测试参考代码 SDK V1.7
 ******************************************************************************
 */

#include "mp3.h"
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include <string.h>

/* ======================== 全局变量 ======================== */
UART_HandleTypeDef MP3_UART_Handler;

/* ======================== 内部函数声明 ======================== */
static void MP3_GPIO_Init(void);
static void MP3_UART_Init(u32 baud);
static void MP3_SendFrame(const uint8_t *data, uint8_t len);
static void MP3_DoChecksum(uint8_t *buf, uint8_t len);

/* ======================== GPIO 初始化 ======================== */

/**
 * @brief  MP3 模块 GPIO 初始化
 *         PA9 -> USART1_TX (AF7)
 *         PA10 -> USART1_RX (AF7)
 */
static void MP3_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    /* PA9, PA10: 复用推挽 + 上拉, AF7 = USART1 */
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* ======================== UART 初始化 ======================== */

/**
 * @brief  MP3 模块 UART 初始化 (9600, 8N1, TX_RX)
 * @param  baud: 波特率
 */
static void MP3_UART_Init(u32 baud)
{
    memset(&MP3_UART_Handler, 0, sizeof(MP3_UART_Handler));
    MP3_UART_Handler.Instance          = USART1;
    MP3_UART_Handler.Init.BaudRate     = baud;
    MP3_UART_Handler.Init.WordLength   = UART_WORDLENGTH_8B;
    MP3_UART_Handler.Init.StopBits     = UART_STOPBITS_1;
    MP3_UART_Handler.Init.Parity       = UART_PARITY_NONE;
    MP3_UART_Handler.Init.Mode         = UART_MODE_TX_RX;
    MP3_UART_Handler.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    MP3_UART_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    MP3_UART_Handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    MP3_UART_Handler.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    MP3_UART_Handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&MP3_UART_Handler) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ======================== 模块初始化 ======================== */

/**
 * @brief  MP3 模块初始化
 * @note   初始化 USART2 (9600bps, PD5/PD6), 选择 TF/SD 卡, 设置默认音量
 */
void MP3_Init(void)
{
    MP3_GPIO_Init();
    MP3_UART_Init(9600);

    /* 等待模块上电稳定 */
    osDelay(500);

    /* 选择播放设备为 TF/SD 卡 */
    MP3_SendCommand(MP3_CMD_SET_DEVICE, 0, MP3_DEVICE_TF);
    osDelay(200);

    /* 设置默认音量 15 */
    MP3_SetVolume(5);
    osDelay(100);
}

/* ======================== 底层校验 ======================== */

/**
 * @brief  计算校验和 (二进制补码取负)
 * @note   Checksum = 0 - sum(buf[0 .. len-1])
 *         结果填入 buf[len] (高字节) 和 buf[len+1] (低字节)
 */
static void MP3_DoChecksum(uint8_t *buf, uint8_t len)
{
    uint16_t sum = 0;
    uint8_t  i;

    for (i = 0; i < len; i++)
    {
        sum += buf[i];
    }
    sum = 0 - sum;

    buf[len]     = (uint8_t)(sum >> 8);
    buf[len + 1] = (uint8_t)(sum & 0xFF);
}

/* ======================== 帧发送 ======================== */

/**
 * @brief  发送一帧完整数据
 * @param  data: 8 字节数据区 (0xFF + 0x06 + CMD + FB + DH + DL + CSH + CSL)
 * @param  len:  恒为 8
 * @note   自动添加帧头 0x7E 和帧尾 0xEF
 */
static void MP3_SendFrame(const uint8_t *data, uint8_t len)
{
    uint8_t buf[12];

    buf[0] = 0x7E;
    memcpy(&buf[1], data, len);
    buf[len + 1] = 0xEF;

    HAL_UART_Transmit(&MP3_UART_Handler, buf, len + 2, 100);
}

/* ======================== 命令发送 (公有) ======================== */

/**
 * @brief  发送命令到 MP3 模块
 * @param  cmd:  命令码 (见 mp3.h 中的 MP3_CMD_xxx 定义)
 * @param  fb:   是否需要应答 (0=不需要, 1=需要)
 * @param  data: 16 位参数
 */
void MP3_SendCommand(u8 cmd, u8 fb, u16 data)
{
    uint8_t frame[8];

    frame[0] = 0xFF;                   /* 版本 */
    frame[1] = 0x06;                   /* 数据长度 */
    frame[2] = cmd;                    /* 命令 */
    frame[3] = fb;                     /* 应答标志 */
    frame[4] = (uint8_t)(data >> 8);   /* 参数高字节 */
    frame[5] = (uint8_t)(data);        /* 参数低字节 */

    MP3_DoChecksum(frame, 6);
    MP3_SendFrame(frame, 8);
}

/* ======================== 控制 API ======================== */

/**
 * @brief  设置音量
 * @param  vol: 0 ~ 30
 */
void MP3_SetVolume(u8 vol)
{
    if (vol > MP3_VOL_MAX) vol = MP3_VOL_MAX;
    MP3_SendCommand(MP3_CMD_SET_VOL, 0, (u16)vol);
}

/**
 * @brief  音量 +1
 */
void MP3_VolumeUp(void)
{
    MP3_SendCommand(MP3_CMD_VOL_UP, 0, 0);
}

/**
 * @brief  音量 -1
 */
void MP3_VolumeDown(void)
{
    MP3_SendCommand(MP3_CMD_VOL_DOWN, 0, 0);
}

/**
 * @brief  播放指定序号的歌曲
 * @param  index: 歌曲序号 1~65535
 *         index=1  -> 001.MP3
 *         index=12 -> 012.MP3
 * @note   切换曲目后自动开始播放
 */
void MP3_PlayIndex(u16 index)
{
    if (index < 1) index = 1;
    MP3_SendCommand(MP3_CMD_PLAY_INDEX, 0, index);
}

/**
 * @brief  播放 / 继续
 */
void MP3_Play(void)
{
    MP3_SendCommand(MP3_CMD_PLAY, 0, 0);
}

/**
 * @brief  暂停播放
 */
void MP3_Pause(void)
{
    MP3_SendCommand(MP3_CMD_PAUSE, 0, 0);
}

/**
 * @brief  停止播放
 * @note   YX5200 无独立停止命令, 通过暂停实现
 */
void MP3_Stop(void)
{
    MP3_Pause();
}

/**
 * @brief  下一曲
 */
void MP3_Next(void)
{
    MP3_SendCommand(MP3_CMD_NEXT_FILE, 0, 0);
}

/**
 * @brief  上一曲
 */
void MP3_Prev(void)
{
    MP3_SendCommand(MP3_CMD_PREV_FILE, 0, 0);
}

/**
 * @brief  开始播放 (等价于 MP3_Play)
 */
void MP3_Start(void)
{
    MP3_Play();
}

/**
 * @brief  指定文件夹播放
 * @param  folder: 文件夹编号 0~99
 *         对应 SD 卡 00/, 01/ ... 99/ 目录
 */
void MP3_PlayFolder(u8 folder)
{
    MP3_SendCommand(MP3_CMD_PLAY_FOLDER, 0, (u16)folder);
}

/**
 * @brief  设置循环模式
 * @param  mode: 播放模式 (MP3_MODE_ALL_LOOP / SINGLE_LOOP / RANDOM / FOLDER_LOOP)
 */
void MP3_SetPlayMode(u8 mode)
{
    MP3_SendCommand(MP3_CMD_SET_PLAYMODE, 0, (u16)mode);
}

/**
 * @brief  设置 EQ 音效
 * @param  eq: MP3_EQ_NORMAL / POP / ROCK / JAZZ / CLASSIC / BASS
 */
void MP3_SetEQ(u8 eq)
{
    MP3_SendCommand(MP3_CMD_SET_EQ, 0, (u16)eq);
}

/**
 * @brief  模块复位
 */
void MP3_Reset(void)
{
    MP3_SendCommand(MP3_CMD_RESET, 0, 0);
    osDelay(500);
}

/* ======================== 查询 API (阻塞轮询) ======================== */

/**
 * @brief  从模块接收一帧应答并返回解析后的数据 (内部使用, 带超时)
 * @param  timeout_ms: 超时时间 (毫秒)
 * @param  out_cmd:    [出参] 模块返回的命令码
 * @param  out_data:   [出参] 模块返回的 16 位数据
 * @return 成功返回 1, 超时/校验失败返回 0
 * @note   模块应答帧格式 (共10字节):
 *         0x7E 0xFF 0x06 CMD FB DH DL CSH CSL 0xEF
 */
static uint8_t MP3_RecvResponse(uint16_t timeout_ms, uint8_t *out_cmd, uint16_t *out_data)
{
    uint8_t  buf[9];   /* 接收 0xFF ~ 0xEF 共 9 字节 */
    uint8_t  byte;
    uint32_t tick_start;

    tick_start = HAL_GetTick();

    /* ---- 第一步: 等待帧头 0x7E ---- */
    while (1)
    {
        if ((HAL_GetTick() - tick_start) > timeout_ms)
            return 0;

        if (HAL_UART_Receive(&MP3_UART_Handler, &byte, 1, 10) != HAL_OK)
            continue;

        if (byte == 0x7E)
            break;
    }

    /* ---- 第二步: 接收剩余 9 字节 ---- */
    if (HAL_UART_Receive(&MP3_UART_Handler, buf, 9, 200) != HAL_OK)
        return 0;

    /* 校验帧尾 */
    if (buf[8] != 0xEF)
        return 0;

    /* 校验 checksum: sum(buf[0..5]) + (buf[6]<<8 | buf[7]) == 0 */
    {
        uint16_t sum = 0;
        uint8_t  i;

        for (i = 0; i < 6; i++)       /* FF + 06 + CMD + FB + DH + DL */
            sum += buf[i];

        sum += ((uint16_t)buf[6] << 8) | buf[7];

        if (sum != 0)
            return 0;
    }

    /* 提取结果 */
    *out_cmd  = buf[2];                      /* 命令码 */
    *out_data = ((uint16_t)buf[4] << 8) | buf[5];  /* 数据 */

    return 1;
}

/**
 * @brief  查询 SD 卡总文件数
 * @return 总文件数, 0xFFFF 表示查询失败
 */
u16 MP3_QueryTotalFiles(void)
{
    uint8_t  cmd;
    uint16_t data;

    MP3_SendCommand(MP3_CMD_QUERY_TF_TOTAL, 0, 0);
    osDelay(50);

    if (!MP3_RecvResponse(500, &cmd, &data))
        return 0xFFFF;

    /* 检查命令码: 0x47(UDisk), 0x48(TF), 0x49(Flash) */
    if (cmd == 0x47 || cmd == 0x48 || cmd == 0x49)
        return data;

    return 0xFFFF;
}

/**
 * @brief  查询当前播放文件序号 (1-based)
 * @return 当前文件序号, 0xFFFF 表示查询失败
 */
u16 MP3_QueryCurrentFile(void)
{
    uint8_t  cmd;
    uint16_t data;

    MP3_SendCommand(MP3_CMD_QUERY_TF_CUR, 0, 0);
    osDelay(50);

    if (!MP3_RecvResponse(500, &cmd, &data))
        return 0xFFFF;

    /* 检查命令码: 0x4B(UDisk), 0x4C(TF), 0x4D(Flash) */
    if (cmd == 0x4B || cmd == 0x4C || cmd == 0x4D)
        return data;

    return 0xFFFF;
}
