#ifndef __MP3_H__
#define __MP3_H__

#include "system.h"

/*******************************************************************************
 * MP3-TF-16P 模块驱动 (YX5200 / YX6300 芯片)
 *
 * 硬件连接:
 *   MCU PA9 (USART1_TX) -> MP3 模块 RX
 *   MCU PA10 (USART1_RX) -> MP3 模块 TX
 *
 * 通信协议:
 *   波特率 9600, 8N1
 *   帧格式: 0x7E + 0xFF + 0x06 + CMD + FB + DATAH + DATAL + CHKSUM_H + CHKSUM_L + 0xEF
 *   校验和: CHKSUM = 0 - (0xFF + 0x06 + CMD + FB + DATAH + DATAL)
 *
 * SD 卡文件命名规则:
 *   001.MP3, 002.MP3, 003.MP3 ... 255.MP3 (放在 SD 卡根目录)
 ******************************************************************************/

/* ======================== 命令码定义 ======================== */

/* ---------- 控制命令 ---------- */
#define MP3_CMD_NEXT_FILE       0x01    /* 下一曲 */
#define MP3_CMD_PREV_FILE       0x02    /* 上一曲 */
#define MP3_CMD_PLAY_INDEX      0x03    /* 指定曲目 (data=曲目序号 1~65535) */
#define MP3_CMD_VOL_UP          0x04    /* 音量+ */
#define MP3_CMD_VOL_DOWN        0x05    /* 音量- */
#define MP3_CMD_SET_VOL         0x06    /* 指定音量 (data=0~30) */
#define MP3_CMD_SET_EQ          0x07    /* 指定 EQ (0=Normal/1=Pop/2=Rock/3=Jazz/4=Classic/5=Bass) */
#define MP3_CMD_SET_PLAYMODE    0x08    /* 指定播放模式 (data: 0=全盘循环/1=单曲循环/2=随机/...) */
#define MP3_CMD_SET_DEVICE      0x09    /* 指定播放设备 (data: 1=UDisk/2=TF/SD卡/4=Flash) */
#define MP3_CMD_SLEEP           0x0A    /* 进入睡眠低功耗 */
#define MP3_CMD_RESET           0x0C    /* 模块复位 */
#define MP3_CMD_PLAY            0x0D    /* 播放/继续 */
#define MP3_CMD_PAUSE           0x0E    /* 暂停 */
#define MP3_CMD_PLAY_FOLDER     0x0F    /* 指定文件夹播放 (data=文件夹名 00~99) */
#define MP3_CMD_PLAY_ALL        0x11    /* 播放全部 (插播广告、Stop 后恢复) */

/* ---------- 查询命令 ---------- */
#define MP3_CMD_QUERY_STATUS    0x42    /* 查询当前状态 */
#define MP3_CMD_QUERY_VOL       0x43    /* 查询当前音量 */
#define MP3_CMD_QUERY_EQ        0x44    /* 查询当前 EQ */
#define MP3_CMD_QUERY_PLAYMODE  0x45    /* 查询播放模式 */
#define MP3_CMD_QUERY_VERSION   0x46    /* 查询版本 */
#define MP3_CMD_QUERY_TF_TOTAL  0x48    /* 查询 TF/SD 卡总文件数 */
#define MP3_CMD_QUERY_TF_CUR    0x4C    /* 查询 TF/SD 卡当前播放文件序号 */

/* ---------- 模块返回的应答/状态码 ---------- */
#define MP3_ACK_SUCCESS         0x41    /* 命令执行成功 ACK */

/* ======================== EQ 定义 ======================== */
#define MP3_EQ_NORMAL           0
#define MP3_EQ_POP              1
#define MP3_EQ_ROCK             2
#define MP3_EQ_JAZZ             3
#define MP3_EQ_CLASSIC          4
#define MP3_EQ_BASS             5

/* ======================== 播放模式定义 ======================== */
#define MP3_MODE_ALL_LOOP       0       /* 全盘循环 */
#define MP3_MODE_SINGLE_LOOP    1       /* 单曲循环 */
#define MP3_MODE_RANDOM         2       /* 随机播放 */
#define MP3_MODE_FOLDER_LOOP    3       /* 文件夹内循环 */

/* ======================== 设备类型定义 ======================== */
#define MP3_DEVICE_UDISK        1       /* U 盘 */
#define MP3_DEVICE_TF           2       /* TF 卡 / SD 卡 */
#define MP3_DEVICE_FLASH        4       /* 板载 Flash */

/* ======================== 音量范围 ======================== */
#define MP3_VOL_MIN             0
#define MP3_VOL_MAX             30

/* ======================== 参数定义 ======================== */
#define MP3_FRAME_LEN           10      /* 完整帧长度 (含头尾) */
#define MP3_DATA_LEN            6       /* 数据段长度 (0xFF ~ DATAL) */
#define MP3_RESEND_MAX          3       /* 最大重发次数 */

/* ======================== 外部变量声明 ======================== */
extern UART_HandleTypeDef MP3_UART_Handler;

/* ======================== API 函数声明 ======================== */

/**
 * @brief  MP3 模块初始化
 * @note   初始化 USART2 (9600bps, TX_RX, PA2/PA3)，
 *         发送设备选择命令切换到 TF/SD 卡
 */
void MP3_Init(void);

/**
 * @brief  设置音量
 * @param  vol: 音量值 (0~30)
 */
void MP3_SetVolume(u8 vol);

/**
 * @brief  音量加 1
 */
void MP3_VolumeUp(void);

/**
 * @brief  音量减 1
 */
void MP3_VolumeDown(void);

/**
 * @brief  播放指定序号的歌曲
 * @param  index: 歌曲序号 (1~65535)
 *         对应 SD 卡根目录下的 001.MP3, 002.MP3, ...
 *         例如: MP3_PlayIndex(1) 播放 001.MP3
 *               MP3_PlayIndex(12) 播放 012.MP3
 */
void MP3_PlayIndex(u16 index);

/**
 * @brief  播放/继续
 */
void MP3_Play(void);

/**
 * @brief  暂停播放
 */
void MP3_Pause(void);

/**
 * @brief  停止播放
 */
void MP3_Stop(void);

/**
 * @brief  下一曲
 */
void MP3_Next(void);

/**
 * @brief  上一曲
 */
void MP3_Prev(void);

/**
 * @brief  开始播放 (与 MP3_Play 相同，用于开始)
 */
void MP3_Start(void);

/**
 * @brief  指定文件夹播放
 * @param  folder: 文件夹编号 (0~99)
 *         对应 SD 卡根目录下的 00/, 01/, ... 99/ 文件夹
 */
void MP3_PlayFolder(u8 folder);

/**
 * @brief  设置播放模式
 * @param  mode: 播放模式
 *         @arg MP3_MODE_ALL_LOOP    全盘循环
 *         @arg MP3_MODE_SINGLE_LOOP 单曲循环
 *         @arg MP3_MODE_RANDOM      随机播放
 *         @arg MP3_MODE_FOLDER_LOOP 文件夹循环
 */
void MP3_SetPlayMode(u8 mode);

/**
 * @brief  设置 EQ 音效
 * @param  eq: EQ 类型
 *         @arg MP3_EQ_NORMAL, MP3_EQ_POP, MP3_EQ_ROCK,
 *              MP3_EQ_JAZZ, MP3_EQ_CLASSIC, MP3_EQ_BASS
 */
void MP3_SetEQ(u8 eq);

/**
 * @brief  查询 SD 卡总文件数 (阻塞, 等待返回)
 * @return 总文件数, 0xFFFF 表示查询失败
 */
u16 MP3_QueryTotalFiles(void);

/**
 * @brief  查询当前播放文件序号 (阻塞, 等待返回)
 * @return 当前文件序号, 0xFFFF 表示查询失败
 */
u16 MP3_QueryCurrentFile(void);

/**
 * @brief  发送原始命令 (高级接口)
 * @param  cmd:  命令码
 * @param  fb:   是否需要应答 (1=需要, 0=不需要)
 * @param  data: 16 位参数
 */
void MP3_SendCommand(u8 cmd, u8 fb, u16 data);

/**
 * @brief  模块复位
 */
void MP3_Reset(void);

#endif /* __MP3_H__ */
