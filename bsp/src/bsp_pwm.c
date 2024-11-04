#include "bsp_pwm.h"
#include "stdlib.h"
#include "memory.h"

#define MAX_PWM_DEVICE_NUM (16)

// Interrupt and initialization support
static uint8_t idx;
static PWM_Instance_t *pwm_instance[MAX_PWM_DEVICE_NUM] = {NULL};

uint32_t PWM_Get_Tclk(TIM_HandleTypeDef *htim);

#ifdef STM32H723xx

uint32_t PWM_Get_Tclk(TIM_HandleTypeDef *htim)
{
    uintptr_t tclk_temp = ((uintptr_t)((htim)->Instance));

    // Check if the timer is on APB1 (e.g., TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14)
    if ((tclk_temp <= (APB1PERIPH_BASE + 0x7FFFUL)) &&
        (tclk_temp >= (APB1PERIPH_BASE + 0x0000UL)))
    {
        // Get APB1 clock frequency
        uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
        uint32_t prescaler = (RCC->D2CFGR & RCC_D2CFGR_D2PPRE1) >> RCC_D2CFGR_D2PPRE1_Pos;

        // Check if the APB1 prescaler is not divided (prescaler = 0 means no division)
        return (prescaler == 0) ? pclk1 : (pclk1 * 2);
    }
    // Check if the timer is on APB2 (e.g., TIM1, TIM8, TIM15, TIM16, TIM17)
    else if ((tclk_temp <= (APB2PERIPH_BASE + 0x3FFFUL)) &&
             (tclk_temp >= (APB2PERIPH_BASE + 0x0000UL)))
    {
        // Get APB2 clock frequency
        uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
        uint32_t prescaler = (RCC->D2CFGR & RCC_D2CFGR_D2PPRE2) >> RCC_D2CFGR_D2PPRE2_Pos;

        // Check if the APB2 prescaler is not divided (prescaler = 0 means no division)
        return (prescaler == 0) ? pclk2 : (pclk2 * 2);
    }

    // Return 0 if timer instance does not match APB1 or APB2
    return 0;
}

#else

// Set the PWM corresponding timer clock source frequency
// tim2~7,12~14:APB1  tim1,8~11:APB2
uint32_t PWM_Get_Tclk(TIM_HandleTypeDef *htim)
{
    uintptr_t tclk_temp = ((uintptr_t)((htim)->Instance));
    if (
        (tclk_temp <= (APB1PERIPH_BASE + 0x2000UL)) &&
        (tclk_temp >= (APB1PERIPH_BASE + 0x0000UL)))
    {
        return (HAL_RCC_GetPCLK1Freq() * (APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos] == 0 ? 1 : 2));
    }
    else if (
        ((tclk_temp <= (APB2PERIPH_BASE + 0x0400UL)) &&
         (tclk_temp >= (APB2PERIPH_BASE + 0x0000UL))) ||
        ((tclk_temp <= (APB2PERIPH_BASE + 0x4800UL)) &&
         (tclk_temp >= (APB2PERIPH_BASE + 0x4000UL))))
    {
        return (HAL_RCC_GetPCLK2Freq() * (APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos] == 0 ? 1 : 2));
    }
    return 0;
}

#endif

PWM_Instance_t *PWM_Register(PWM_Config_t *config)
{
    if (idx >= MAX_PWM_DEVICE_NUM)
        while (1)
            ;
    PWM_Instance_t *pwm = (PWM_Instance_t *)malloc(sizeof(PWM_Instance_t));

    pwm->htim = config->htim;
    pwm->channel = config->channel;
    pwm->period = config->period;
    pwm->dutyratio = config->dutyratio;
    pwm->id = config->id;
    pwm->tclk = PWM_Get_Tclk(pwm->htim);

    // Start PWM
    PWM_Start(pwm);
    PWM_Set_Period(pwm, pwm->period);
    PWM_Set_Duty_Ratio(pwm, pwm->dutyratio);
    pwm_instance[idx++] = pwm;
    return pwm;
}

/* Only provides a formal encapsulation of HAL functions */
void PWM_Start(PWM_Instance_t *pwm)
{
    HAL_TIM_PWM_Start(pwm->htim, pwm->channel);
}

/* Only provides a formal encapsulation of HAL functions */
void PWM_Stop(PWM_Instance_t *pwm)
{
    HAL_TIM_PWM_Stop(pwm->htim, pwm->channel);
}

/*
 * @brief Set the PWM period
 *
 * @param pwm PWM instance
 * @param freq Frequency in Hz
 */
void PWM_Set_Frequency(PWM_Instance_t *pwm, float freq)
{
    __HAL_TIM_SetAutoreload(pwm->htim, (pwm->tclk) / (freq * (pwm->htim->Init.Prescaler + 1)));
}

/*
 * @brief Set the PWM period
 *
 * @param pwm PWM instance
 * @param period Period in seconds
 */
void PWM_Set_Period(PWM_Instance_t *pwm, float period)
{
    __HAL_TIM_SetAutoreload(pwm->htim, period * ((pwm->tclk) / (pwm->htim->Init.Prescaler + 1)));
}

/*
 * @brief Set the PWM duty ratio
 *
 * @param pwm PWM instance
 * @param dutyratio Duty ratio from 0 to 1
 */
void PWM_Set_Duty_Ratio(PWM_Instance_t *pwm, float dutyratio)
{
    __HAL_TIM_SetCompare(pwm->htim, pwm->channel, dutyratio * (pwm->htim->Instance->ARR));
}

