#include "chassis_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "referee_system.h"
#include "Swerve_Locomotion.h"
#include "motion_profile.h"

extern Robot_State_t g_robot_state;
extern Chassis_State_t g_chassis_state;
extern Remote_t g_remote;
extern IMU_t g_imu;

#define CHASSIS_MAX_SPEED_METERS (1.5f)
#define CHASSIS_POWER_BUFFER_MAX_JOULES (60.0f)
#define CHASSIS_CURRENT_MAX_AMPS (120.0f)
#define SPIN_TOP_OMEGA (1.0f)

#define CHASSIS_TRAPEZOID_PROFILE 0.5, 0.5, 1 // acceleration, deceleration, max speed

float chassis_rad;

Trapezoid_Profile_t x_speed_profile;
Trapezoid_Profile_t y_speed_profile;
Trapezoid_Profile_t omega_profile;

static uint32_t last_time;

void Chassis_Task_Init()
{
    Swerve_Init();
    Trapezoid_Profile_Init(&x_speed_profile, CHASSIS_TRAPEZOID_PROFILE);
    Trapezoid_Profile_Init(&y_speed_profile, CHASSIS_TRAPEZOID_PROFILE);
    Trapezoid_Profile_Init(&omega_profile, CHASSIS_TRAPEZOID_PROFILE);

    chassis_rad = TRACK_WIDTH * 0.5f * sqrt(2);
    last_time = HAL_GetTick();
}

void Chassis_Ctrl_Loop()
{
    if (g_robot_state.enabled)
    {
        uint32_t current_time = HAL_GetTick();
        float delta_time = (current_time - last_time) / 1000.0f; // DT in seconds
        last_time = current_time;

        if (delta_time <= 0) // prevent division by zero and overflow issues
        {
            delta_time = 0.001f;
        }

        Trapezoid_Profile_Set_Target(&x_speed_profile, g_chassis_state.x_speed);
        Trapezoid_Profile_Set_Target(&y_speed_profile, g_chassis_state.y_speed);
        Trapezoid_Profile_Set_Target(&y_speed_profile, g_chassis_state.omega);

        float profiled_x_speed = Trapezoid_Profile_Update(&x_speed_profile, delta_time);
        float profiled_y_speed = Trapezoid_Profile_Update(&y_speed_profile, delta_time);
        float profiled_omega = Trapezoid_Profile_Update(&omega_profile, delta_time);

        /* power buffer*/
        float power_buffer_frac = Referee_System.Power_Heat.Chassis_Power_Buffer / CHASSIS_POWER_BUFFER_MAX_JOULES;
        float current_draw_frac = Referee_System.Power_Heat.Chassis_Current / CHASSIS_CURRENT_MAX_AMPS;
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
            Trapezoid_Profile_Set_Target(&omega_profile, SPIN_TOP_OMEGA);
            profiled_omega = Trapezoid_Profile_Update(&omega_profile, delta_time);

            float translation_speed = sqrt(pow(profiled_x_speed, 2) + pow(profiled_y_speed, 2));
            float spin_coeff = chassis_rad * SPIN_TOP_OMEGA / (translation_speed + chassis_rad * SPIN_TOP_OMEGA);

            profiled_omega = profiled_omega * spin_coeff;
        }
        else
        {
            profiled_omega = 0;
        }

        Swerve_Drive(profiled_x_speed, profiled_y_speed, profiled_omega, g_chassis_state.max_speed);
    }
    else
    {
        Swerve_Disable();
    }
}