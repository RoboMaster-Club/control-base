/**
 * @file Referee_System.c
 * @author Leo Liu
 * @brief reading data from the referee system
 * @version 2.0
 * @date 2024-05-22
 *
 * @copyright Copyright (c) 2024
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

    Referee_Robot_State.Cooling_Rate = SENTRY_COOLING_RATE;
    Referee_Robot_State.Heat_Max = SENTRY_HEAT_MAX;
    Referee_Robot_State.Launch_Speed_Max = SENTRY_LAUNCH_SPEED_MAX;
    Referee_Robot_State.Chassis_Power_Max = SENTRY_POWER_MAX;

    Referee_Robot_State.Chassis_Power = Referee_System.Power_Heat.Chassis_Power;
    Referee_Robot_State.Power_Buffer = Referee_System.Power_Heat.Chassis_Power_Buffer;
    Referee_Robot_State.Shooter_Heat_1 = Referee_System.Power_Heat.Shooter_1_17mm_Heat;
    Referee_Robot_State.Shooter_Heat_2 = Referee_System.Power_Heat.Shooter_2_17mm_Heat;
    Referee_Robot_State.Shooting_Frequency = Referee_System.Shooter.Frequency;
    Referee_Robot_State.Shooting_Speed = Referee_System.Shooter.Speed;
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
                    memcpy(&Referee_System.Game_Status, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_GAME_STATUS_LEN-9]));
                    n += REFEREE_GAME_STATUS_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_GAME_RESULT:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_GAME_RESULT_LEN))
                {
                    memcpy(&Referee_System.Game_Result, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_GAME_RESULT_LEN-9]));
                    n += REFEREE_GAME_RESULT_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_HP:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_HP_LEN))
                {
                    memcpy(&Referee_System.Alive_Robot, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_ROBOT_HP_LEN-9]));
                    n += REFEREE_ROBOT_HP_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_EVENT_DATA:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_EVENT_DATA_LEN))
                {
                    memcpy(&Referee_System.Event, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_EVENT_DATA_LEN-9]));
                    n += REFEREE_EVENT_DATA_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_SUPPLY_STATION:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_SUPPLY_STATION_LEN))
                {
                    memcpy(&Referee_System.Supply_Station, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_SUPPLY_STATION_LEN-9]));
                    n += REFEREE_SUPPLY_STATION_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_REFEREE_WARNING:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_REFEREE_WARNING_LEN))
                {
                    memcpy(&Referee_System.Warning, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_REFEREE_WARNING_LEN-9]));
                    n += REFEREE_REFEREE_WARNING_LEN;
                }
                else
                    n++;
                break;
		    case REFEREE_DART_DATA:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_DART_DATA_LEN))
                {
                    memcpy(&Referee_System.Dart_Info, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_DART_DATA_LEN-9]));
                    n += REFEREE_DART_DATA_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_STATE:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_STATE_LEN))
                {
                    memcpy(&Referee_System.Robot_State, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_ROBOT_STATE_LEN-9]));
                    n += REFEREE_ROBOT_STATE_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_POWER_HEAT:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_POWER_HEAT_LEN))
                {
                    memcpy(&Referee_System.Power_Heat, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_POWER_HEAT_LEN-9]));
                    n += REFEREE_POWER_HEAT_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_POSITION:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_POSITION_LEN))
                {
                    memcpy(&Referee_System.Robot_Pos, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_ROBOT_POSITION_LEN-9]));
                    n += REFEREE_ROBOT_POSITION_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_BUFF:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_BUFF_LEN))
                {
                    memcpy(&Referee_System.Robot_Buff, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_ROBOT_BUFF_LEN-9]));
                    n += REFEREE_ROBOT_BUFF_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_AERIAL_DATA:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_AERIAL_DATA))
                {
                    memcpy(&Referee_System.Aerial_Data, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_AERIAL_DATA_LEN-9]));
                    n += REFEREE_AERIAL_DATA;
                }
                else
                    n++;
                break;
            case REFEREE_INJURY_STATE:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_INJURY_STATE_LEN))
                {
                    memcpy(&Referee_System.Injury_State, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_INJURY_STATE_LEN-9]));
                    n += REFEREE_INJURY_STATE_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_SHOOTER_STATE:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_SHOOTER_STATE_LEN))
                {
                    memcpy(&Referee_System.Shooter, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_SHOOTER_STATE_LEN-9]));
                    n += REFEREE_SHOOTER_STATE_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_REMAINING_AMMO:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_REMAINING_AMMO_LEN))
                {
                    memcpy(&Referee_System.Remaining_Ammo, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_REMAINING_AMMO_LEN-9]));
                    n += REFEREE_REMAINING_AMMO_LEN;
                }
                else
                    n++;
                break;
            case REFEREE_ROBOT_RFID:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_ROBOT_RFID_LEN))
                {
                    memcpy(&Referee_System.RFID, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_ROBOT_RFID_LEN-9]));
                    n += REFEREE_ROBOT_RFID_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_DART_CLIENT:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_DART_CLIENT_LEN))
                {
                    memcpy(&Referee_System.Dart_Client, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_DART_CLIENT_LEN-9]));
                    n += REFEREE_DART_CLIENT_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_GROUND_POS:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_GROUND_POS_LEN))
                {
                    memcpy(&Referee_System.Ground_Pos, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_GROUND_POS_LEN-9]));
                    n += REFEREE_GROUND_POS_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_RADAR_PROGRESS:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_RADAR_PROGRESS_LEN))
                {
                    memcpy(&Referee_System.Radar_Progress, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_RADAR_PROGRESS_LEN-9]));
                    n += REFEREE_RADAR_PROGRESS_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_SENTRY_DECISION:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_SENTRY_DECISION_LEN))
                {
                    memcpy(&Referee_System.Sentry_Decision, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_SENTRY_DECISION_LEN-9]));
                    n += REFEREE_SENTRY_DECISION_LEN;
                }
                else
                    n++;
                break;
			case REFEREE_RADAR_DECISION:
                if (Verify_CRC16_Check_Sum(Referee_System.Buffer + n, REFEREE_RADAR_DECISION_LEN))
                {
                    memcpy(&Referee_System.Radar_Decision, &Referee_System.Buffer[n + 7], sizeof(uint8_t[REFEREE_RADAR_DECISION_LEN-9]));
                    n += REFEREE_RADAR_DECISION_LEN;
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