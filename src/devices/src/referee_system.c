/**
 * @file Referee_System.c
 * @author Leo Liu
 * @brief reading data from the referee system
 * @version 1.0
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "referee_system.h"
#include "usart.h"

Referee_System_t Referee_System;
Referee_Robot_State_t Referee_Robot_State;

void Referee_Get_Data(void);
void Referee_Set_Robot_State(void);

void Referee_Set_Robot_State(void)
{
    Referee_Robot_State.Game_Type = Referee_System.Game_Status.Type;
    Referee_Robot_State.ID = Referee_System.Robot_State.ID;
    Referee_Robot_State.Level = Referee_System.Robot_State.Level;

    Referee_Robot_State.Cooling_Rate = Referee_System.Robot_State.Shooter_1_Cooling_Rate_17mm;
    Referee_Robot_State.Heat_Max = Referee_System.Robot_State.Shooter_1_Heat_Max_17mm;
    Referee_Robot_State.Bullet_Speed_Max = Referee_System.Robot_State.Shooter_1_Speed_Limit_17mm;
    Referee_Robot_State.Chassis_Power_Max = Referee_System.Robot_State.Chassis_Power_Max;

    Referee_Robot_State.Chassis_Power = Referee_System.Power_n_Heat.Chassis_Power;
    Referee_Robot_State.Power_Buffer = Referee_System.Power_n_Heat.Chassis_Power_Buffer;
    Referee_Robot_State.Shooter_Heat = Referee_System.Power_n_Heat.Shooter_1_Heat;
    Referee_Robot_State.Shooting_Frequency = Referee_System.Shooter.Frequency;
    Referee_Robot_State.Shooting_Speed = Referee_System.Shooter.Speed;

    // TODO: test fric speed + chassis spintop power
    // switch(Referee_Robot_State.Bullet_Speed_Max)
    // {
    // 	case 15:
    // 		Shooting.Fric_Wheel.Target_Speed = FRIC_SPEED_15;
    // 		break;
    // 	case 18:
    // 		Shooting.Fric_Wheel.Target_Speed = FRIC_SPEED_18;
    // 		break;
    // 	case 30:
    // 		Shooting.Fric_Wheel.Target_Speed = FRIC_SPEED_30;
    // 		break;
    // 	default:
    // 		Shooting.Fric_Wheel.Target_Speed = FRIC_SPEED_10;
    // 		break;
    // }

    // switch(Referee_Robot_State.Chassis_Power_Max)
    // {
    // 	case 60:
    // 		Chassis.Chassis_Coord.Spin_Rate = CHASSIS_SPINTOP_RATE_POWER_60;
    // 		break;
    // 	case 80:
    // 		Chassis.Chassis_Coord.Spin_Rate = CHASSIS_SPINTOP_RATE_POWER_80;
    // 		break;
    // 	case 100:
    // 		Chassis.Chassis_Coord.Spin_Rate = CHASSIS_SPINTOP_RATE_POWER_100;
    // 		break;
    // 	case 150:
    // 		Chassis.Chassis_Coord.Spin_Rate = CHASSIS_SPINTOP_RATE_POWER_150;
    // 		break;
    // 	default:
    // 		Chassis.Chassis_Coord.Spin_Rate = CHASSIS_SPINTOP_RATE_POWER_60;
    // 		break;
    // }
}

void Referee_System_Init(UART_HandleTypeDef *huart)
{
    Referee_System.huart = huart;
    HAL_UART_Receive_DMA(huart, Referee_System.Buffer, REFEREE_BUFFER_LEN);
}

// Get referee system data based on ID
void Referee_Get_Data(void)
{
    for (int n = 0; n < REFEREE_BUFFER_LEN;)
    {
        if (Referee_System.Buffer[n] == REFEREE_FRAME_HEADER_START)
        {
            switch (Referee_System.Buffer[n + 5] | Referee_System.Buffer[n + 6] << 8)
            {
            case REFEREE_GAME_STATUS:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_GAME_STATUS_LEN))
                {
                    memcpy(&Referee_System.Game_Status, &Referee_System.Buffer[n + 7], sizeof(uint8_t[11]));
                    n += REFEREE_GAME_STATUS_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_GAME_RESULT:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_GAME_RESULT_LEN))
                {
                    memcpy(&Referee_System.Game_Result, &Referee_System.Buffer[n + 7], sizeof(uint8_t[1]));
                    n += REFEREE_GAME_RESULT_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_HP:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_HP_LEN))
                {
                    memcpy(&Referee_System.Alive_Robot, &Referee_System.Buffer[n + 7], sizeof(uint8_t[32]));
                    n += REFEREE_ROBOT_HP_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_EVENT_DATA:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_EVENT_DATA_LEN))
                {
                    memcpy(&Referee_System.Event, &Referee_System.Buffer[n + 7], sizeof(uint8_t[4]));
                    n += REFEREE_EVENT_DATA_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_REFEREE_WARNING:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_REFEREE_WARNING_LEN))
                {
                    memcpy(&Referee_System.Warning, &Referee_System.Buffer[n + 7], sizeof(uint8_t[2]));
                    n += REFEREE_REFEREE_WARNING_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_STATE:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_STATE_LEN))
                {
                    memcpy(&Referee_System.Robot_State, &Referee_System.Buffer[n + 7], sizeof(uint8_t[27]));
                    n += REFEREE_ROBOT_STATE_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_POWER_HEAT:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_POWER_HEAT_LEN))
                {
                    memcpy(&Referee_System.Power_n_Heat, &Referee_System.Buffer[n + 7], sizeof(uint8_t[16]));
                    n += REFEREE_POWER_HEAT_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_POSITION:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_POSITION_LEN))
                {
                    memcpy(&Referee_System.Location, &Referee_System.Buffer[n + 7], sizeof(uint8_t[16]));
                    n += REFEREE_ROBOT_POSITION_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_BUFF:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_BUFF_LEN))
                {
                    memcpy(&Referee_System.Robot_Buff, &Referee_System.Buffer[n + 7], sizeof(uint8_t[1]));
                    n += REFEREE_ROBOT_BUFF_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_INJURY_STATE:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_INJURY_STATE_LEN))
                {
                    memcpy(&Referee_System.Damage, &Referee_System.Buffer[n + 7], sizeof(uint8_t[1]));
                    n += REFEREE_INJURY_STATE_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_SHOOTER_STATE:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_SHOOTER_STATE_LEN))
                {
                    memcpy(&Referee_System.Shooter, &Referee_System.Buffer[n + 7], sizeof(uint8_t[7]));
                    n += REFEREE_SHOOTER_STATE_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_REMAINING_AMMO:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_REMAINING_AMMO_LEN))
                {
                    memcpy(&Referee_System.Remaining_Ammo, &Referee_System.Buffer[n + 7], sizeof(uint8_t[6]));
                    n += REFEREE_REMAINING_AMMO_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_RFID:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_RFID_LEN))
                {
                    memcpy(&Referee_System.RFID, &Referee_System.Buffer[n + 7], sizeof(uint8_t[4]));
                    n += REFEREE_ROBOT_RFID_LEN;
                }
                else
                    n++;
                break;
            default:
                n++;
                break;
            }
        }
        else
            n++;
    }
}