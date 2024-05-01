#include "chassis_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "referee_system.h"
#include "Swerve_Locomotion.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;

#define CHASSIS_MAX_SPEED_METERS (2.0f)
#define CHASSIS_POWER_BUFFER_MAX_JOULES (60.0f)
#define CHASSIS_CURRENT_MAX_AMPS (120.0f)
#define SPINTOP_COEF (0.003f)
#define SPIN_TOP_OMEGA (1.0f)

void Chassis_Task_Init()
{
    Swerve_Init();
}

void Chassis_Ctrl_Loop()
{
    if (g_robot_state.enabled)
    {
        /* power buffer*/
        float power_buffer_frac = Referee_System.Power_n_Heat.Chassis_Power_Buffer / CHASSIS_POWER_BUFFER_MAX_JOULES;
        float current_draw_frac = Referee_System.Power_n_Heat.Chassis_Current / CHASSIS_CURRENT_MAX_AMPS;
        float speed_limitter = (current_draw_frac + pow(power_buffer_frac, 2)) / 2;

        if (current_draw_frac < 0.9f)
        {
            g_robot_state.chassis_max_speed = CHASSIS_MAX_SPEED_METERS * speed_limitter;
        }
        else
        {
            g_robot_state.chassis_max_speed = CHASSIS_MAX_SPEED_METERS;
        }

        /*
         * scale spintop omega by inverse of translation speed to prioritize translation
         * spin_coeff = rw/(v + rw) // r = rad, w = desired omega (spin top omega), v = translational speed
         * chassis_omega *= spin_coeff
         */
        if (g_robot_state.spintop_mode)
        {
            float translation_speed = sqrt(pow(g_robot_state.chassis_x_speed, 2) + pow(g_robot_state.chassis_y_speed, 2));
            float chassis_rad = TRACK_WIDTH * 0.5f * sqrt(2);
            float spin_coeff = chassis_rad * SPIN_TOP_OMEGA / (translation_speed + chassis_rad * SPIN_TOP_OMEGA);
            g_robot_state.chassis_omega = SPIN_TOP_OMEGA * spin_coeff;
        }

        Swerve_Drive(g_robot_state.chassis_x_speed, g_robot_state.chassis_y_speed, g_robot_state.chassis_omega, g_robot_state.chassis_max_speed);
    }
    else
    {
        Swerve_Disable();
    }
}