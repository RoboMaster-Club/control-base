#include "supercap.h"

Supercap_t g_supercap;
CAN_Instance_t *supercap_can_instance;
extern Jetson_Orin_Data_t g_orin_data;

void Supercap_Init(Supercap_t *g_supercap)
{
    // Initialize supercap
    g_supercap->can_bus = 1;
    g_supercap->tx_id = 0x166;
    g_supercap->rx_id = 0x188;
    supercap_can_instance = CAN_Device_Register(g_supercap->can_bus,g_supercap->tx_id,g_supercap->rx_id,Supercap_Decode);
}

void Supercap_Decode(CAN_Instance_t *can_instance)
{
    // Send supercap data
    uint8_t *data = can_instance->rx_buffer;
    g_supercap.supercap_percent = (data[0]-30)*3.0f;
}

void Supercap_Send(void)
{
    // Send supercap data
    uint8_t *data = supercap_can_instance->tx_buffer;
    if (g_orin_data.sending.game_start_flag)
    {
        data[0] = Referee_Robot_State.Chassis_Power_Max;
    }
    else
    {
        data[0] = 90;
    }
    
    CAN_Transmit(supercap_can_instance);
}