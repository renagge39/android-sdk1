#include "stm32f1xx.h"

void init_peripherals() {
    HAL_Init();
    init_suhu_sensor();
    init_ph_sensor();
    init_DO_sensor();
    init_pwm_aerator();
    init_timer_interrupt();
    init_uart();
}

void control_aerator(float do_level) {
    if (do_level < 4.0) set_pwm_aerator(100);
    else if (do_level < 6.0) set_pwm_aerator(50);
    else set_pwm_aerator(0);
}

void control_pakan() {
    if (check_waktu_pakan()) beri_pakan();
}

void EXTI15_10_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13)) {
        beri_pakan();
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
    }
}

int main() {
    init_peripherals();
    
    while (1) {
        float suhu = read_suhu_dma();
        float ph = read_ph_dma();
        float do_level = read_DO_dma();

        control_aerator(do_level);
        control_pakan();

        kirim_data_UART(suhu, ph, do_level);
        HAL_Delay(500);
    }
}
