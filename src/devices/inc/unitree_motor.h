#ifndef UNITREE_MOTOR_H
#define UNITREE_MOTOR_H

#include <stdint.h>
#include "main.h"

/*
 * This mysterious table is just the CRC of each possible byte. It can be
 * computed using the standard bit-at-a-time methods. The polynomial can
 * be seen in entry 128, 0x8408. This corresponds to x^0 + x^5 + x^12.
 * Add the implicit x^16, and you have the standard CRC-CCITT.
 * https://github.com/torvalds/linux/blob/5bfc75d92efd494db37f5c4c173d3639d4772966/lib/crc-ccitt.c
 */
uint16_t const crc_ccitt_table[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78};

static uint16_t crc_ccitt_byte(uint16_t crc, const uint8_t c)
{
    return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

/**
 *	crc_ccitt - recompute the CRC (CRC-CCITT variant) for the data
 *	buffer
 *	@crc: previous CRC value
 *	@buffer: data pointer
 *	@len: number of bytes in the buffer
 */
static uint16_t crc_ccitt(uint16_t crc, uint8_t const *buffer, size_t len)
{
    uint16_t tmp = crc;
    while (len--)
        tmp = crc_ccitt_byte(tmp, *buffer++);
    return tmp;
}

#pragma pack(1)

/**
 * @brief 电机模式控制信息
 *
 */
typedef struct
{
    uint8_t id : 4;     // 电机ID: 0,1...,13,14 15表示向所有电机广播数据(此时无返回)
    uint8_t status : 3; // 工作模式: 0.锁定 1.FOC闭环 2.编码器校准 3.保留
    uint8_t none : 1;   // 保留位
} RIS_Mode_t;           // 控制模式 1Byte

/**
 * @brief 电机状态控制信息
 *
 */
typedef struct
{
    int16_t tor_des; // 期望关节输出扭矩 unit: N.m      (q8)
    int16_t spd_des; // 期望关节输出速度 unit: rad/s    (q8)
    int32_t pos_des; // 期望关节输出位置 unit: rad      (q15)
    int16_t k_pos;   // 期望关节刚度系数 unit: -1.0-1.0 (q15)
    int16_t k_spd;   // 期望关节阻尼系数 unit: -1.0-1.0 (q15)

} RIS_Comd_t; // 控制参数 12Byte

/**
 * @brief 电机状态反馈信息
 *
 */
typedef struct
{
    int16_t torque;      // 实际关节输出扭矩 unit: N.m     (q8)
    int16_t speed;       // 实际关节输出速度 unit: rad/s   (q8)
    int32_t pos;         // 实际关节输出位置 unit: rad     (q15)
    int8_t temp;         // 电机温度: -128~127°C
    uint8_t MError : 3;  // 电机错误标识: 0.正常 1.过热 2.过流 3.过压 4.编码器故障 5-7.保留
    uint16_t force : 12; // 足端气压传感器数据 12bit (0-4095)
    uint8_t none : 1;    // 保留位
} RIS_Fbk_t;             // 状态数据 11Byte

#pragma pack(1)
typedef union
{
    int32_t L;
    uint8_t u8[4];
    uint16_t u16[2];
    uint32_t u32;
    float F;
} COMData32;

typedef struct
{
    // 定义 数据包头
    unsigned char start[2]; // 包头
    unsigned char motorID;  // 电机ID  0,1,2,3 ...   0xBB 表示向所有电机广播（此时无返回）
    unsigned char reserved;
} COMHead;

typedef struct
{ // 以 4个字节一组排列 ，不然编译器会凑整
    // 定义 数据
    uint8_t mode;    // 当前关节模式
    uint8_t ReadBit; // 电机控制参数修改     是否成功位
    int8_t Temp;     // 电机当前平均温度
    uint8_t MError;  // 电机错误 标识

    COMData32 Read; // 读取的当前 电机 的控制数据
    int16_t T;      // 当前实际电机输出力矩       7 + 8 描述

    int16_t W; // 当前实际电机速度（高速）   8 + 7 描述
    float LW;  // 当前实际电机速度（低速）

    int16_t W2; // 当前实际关节速度（高速）   8 + 7 描述
    float LW2;  // 当前实际关节速度（低速）

    int16_t Acc;    // 电机转子加速度       15+0 描述  惯量较小
    int16_t OutAcc; // 输出轴加速度         12+3 描述  惯量较大

    int32_t Pos;  // 当前电机位置（主控0点修正，电机关节还是以编码器0点为准）
    int32_t Pos2; // 关节编码器位置(输出编码器)

    int16_t gyro[3]; // 电机驱动板6轴传感器数据
    int16_t acc[3];

    // 力传感器的数据
    int16_t Fgyro[3];
    int16_t Facc[3];
    int16_t Fmag[3];
    uint8_t Ftemp; // 8位表示的温度  7位（-28~100度）  1位0.5度分辨率

    int16_t Force16; // 力传感器高16位数据
    int8_t Force8;   // 力传感器低8位数据

    uint8_t FError; //  足端传感器错误标识

    int8_t Res[1]; // 通讯 保留字节

} ServoComdV3; // 加上数据包的包头 和CRC 78字节（4+70+4）

typedef struct
{
    uint8_t head[2]; // 包头         2Byte
    RIS_Mode_t mode; // 电机控制模式  1Byte
    RIS_Fbk_t fbk;   // 电机反馈数据 11Byte
    uint16_t CRC16;  // CRC          2Byte
} MotorData_t;       // 返回数据

typedef struct
{
    uint8_t none[8]; // 保留

} LowHzMotorCmd;

typedef struct
{                      // 以 4个字节一组排列 ，不然编译器会凑整
                       // 定义 数据
    uint8_t mode;      // 关节模式选择
    uint8_t ModifyBit; // 电机控制参数修改位
    uint8_t ReadBit;   // 电机控制参数发送位
    uint8_t reserved;

    COMData32 Modify; // 电机参数修改 的数据
    // 实际给FOC的指令力矩为：
    //  K_P*delta_Pos + K_W*delta_W + T
    int16_t T;   // 期望关节的输出力矩（电机本身的力矩）x256, 7 + 8 描述
    int16_t W;   // 期望关节速度 （电机本身的速度） x128,       8 + 7描述
    int32_t Pos; // 期望关节位置 x 16384/6.2832, 14位编码器（主控0点修正，电机关节还是以编码器0点为准）

    int16_t K_P; // 关节刚度系数 x2048  4+11 描述
    int16_t K_W; // 关节速度系数 x1024  5+10 描述

    uint8_t LowHzMotorCmdIndex; // 保留
    uint8_t LowHzMotorCmdByte;  // 保留

    COMData32 Res[1]; // 通讯 保留字节  用于实现别的一些通讯内容

} MasterComdV3; // 加上数据包的包头 和CRC 34字节

typedef struct
{
    // 定义 电机控制命令数据包
    uint8_t head[2]; // 包头         2Byte
    RIS_Mode_t mode; // 电机控制模式  1Byte
    RIS_Comd_t comd; // 电机期望数据 12Byte
    uint16_t CRC16;  // CRC          2Byte
} ControlData_t;     // 电机控制命令数据包

#pragma pack()

typedef struct
{
    // 定义 发送格式化数据
    ControlData_t motor_send_data; // 电机控制数据结构体
    int hex_len;                   // 发送的16进制命令数组长度, 34
    long long send_time;           // 发送该命令的时间, 微秒(us)
    // 待发送的各项数据
    unsigned short id;   // 电机ID，0代表全部电机
    unsigned short mode; // 0:空闲, 5:开环转动, 10:闭环FOC控制
    // 实际给FOC的指令力矩为：
    //  K_P*delta_Pos + K_W*delta_W + T
    float T;       // 期望关节的输出力矩（电机本身的力矩）（Nm）
    float W;       // 期望关节速度（电机本身的速度）(rad/s)
    float Pos;     // 期望关节位置（rad）
    float K_P;     // 关节刚度系数
    float K_W;     // 关节速度系数
    COMData32 Res; // 通讯 保留字节  用于实现别的一些通讯内容
} MOTOR_send;

typedef struct
{
    // 定义 接收数据
    MotorData_t motor_recv_data; // 电机接收数据结构体，详见motor_msg.h
    int hex_len;                 // 接收的16进制命令数组长度, 78
    long long resv_time;         // 接收该命令的时间, 微秒(us)
    int correct;                 // 接收数据是否完整（1完整，0不完整）
    // 解读得出的电机数据
    unsigned char motor_id; // 电机ID
    unsigned char mode;     // 0:空闲, 5:开环转动, 10:闭环FOC控制
    int Temp;               // 温度
    unsigned char MError;   // 错误码
    float T;                // 当前实际电机输出力矩
    float W;                // speed
    float Pos;              // 当前电机位置（主控0点修正，电机关节还是以编码器0点为准）
    float footForce;        // 足端气压传感器数据 12bit (0-4095)

} MOTOR_recv;

typedef struct _Unitree_Motor
{
    uint8_t uart;
    uint8_t motor_id;
    MOTOR_send motor_send;
    MOTOR_recv motor_recv;
} Unitree_Motor;

uint32_t crc32_core(uint32_t *ptr, uint32_t len);
int modify_data(MOTOR_send *motor_s);
int extract_data(MOTOR_recv *motor_r);
HAL_StatusTypeDef SERVO_Send_recv(MOTOR_send *pData, MOTOR_recv *rData);

#pragma pack()

#endif // UNITREE_MOTOR_H