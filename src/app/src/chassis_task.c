#include "chassis_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "referee_system.h"
#include "Swerve_Locomotion.h"

extern Robot_State_t g_robot_state;
extern Chassis_State_t g_chassis_state;
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
        float speed_limiter = (current_draw_frac + pow(power_buffer_frac, 2)) / 2;

        g_chassis_state.power_buffer_frac = power_buffer_frac;
        g_chassis_state.current_draw_frac = current_draw_frac;
        g_chassis_state.speed_limiter = speed_limiter;

        if (current_draw_frac < 0.9f)
        {
            g_chassis_state.max_speed = CHASSIS_MAX_SPEED_METERS * speed_limiter;
        }
        else
        {
            g_chassis_state.max_speed = CHASSIS_MAX_SPEED_METERS;
        }

        /*
         * scale spintop omega by inverse of translation speed to prioritize translation
         * spin_coeff = rw/(v + rw) // r = rad, w = desired omega (spin top omega), v = translational speed
         * chassis_omega *= spin_coeff
         */
        if (g_robot_state.spintop_mode)
        {
            float translation_speed = sqrt(pow(g_chassis_state.x_speed, 2) + pow(g_chassis_state.y_speed, 2));
            float chassis_rad = TRACK_WIDTH * 0.5f * sqrt(2);
            float spin_coeff = chassis_rad * SPIN_TOP_OMEGA / (translation_speed + chassis_rad * SPIN_TOP_OMEGA);
            g_chassis_state.omega = SPIN_TOP_OMEGA * spin_coeff;
        }

        Swerve_Drive(g_chassis_state.x_speed, g_chassis_state.y_speed, g_chassis_state.omega, g_chassis_state.max_speed);
    }
    else
    {
        Swerve_Disable();
    }
}