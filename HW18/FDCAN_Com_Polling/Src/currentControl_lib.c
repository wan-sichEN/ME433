#include "currentControl_lib.h"

#define KP_CURRENT 0.5
#define KI_CURRENT 0.5

volatile uint8_t state = 0;
volatile signed short current_array[MAX_SAMPLES];
volatile signed short desired_array[MAX_SAMPLES];
volatile float desired_current;

float read_ina219();
void writeINA219(int reg, int value);
signed short readINA219(unsigned char reg);
void set_motor_pwm(int pwm);


void print_STM32(char *message){
	HAL_UART_Transmit(&hcom_uart[COM1], (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

uint32_t read_ADC(void){
	uint32_t raw;
	HAL_ADC_Start(&hadc1);                                 // start conversion
	if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)  // wait (10 ms timeout)
	{
		raw = HAL_ADC_GetValue(&hadc1);                   // read raw ADC value
	}
	else {
		raw = 0;
	}
	HAL_ADC_Stop(&hadc1);
	return raw;
}

// timer based interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if (htim == &htim2) {
		// do your interrupt stuff here
//		static volatile float eint = 0.0f;
//		static volatile int counter = 0;
//		static volatile int nth_sample = 0;
//
    	// safety
    	uint32_t adc_raw = read_ADC();
    	if (adc_raw < 250 || adc_raw > 4095 - 250){
    		// turn motor off
    		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2400);
    		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2400);
    	}
//
//    	// current control
//		signed short current = read_ina219();
//
//		// PI Control
//		float error = desired_current - current;
//		eint = eint + error;
//		float u = KP_CURRENT*error + KI_CURRENT*eint;
//		set_motor_pwm((int)u);
    }
}

void init_PWM_STM32(void){
	// motor is off when both pins are high (2400)
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2400);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2400);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void init_ina219(){
    // set the INA219 sensitivity - 10 bit, plus/minus160mV, 148us per sample
    unsigned short ina219_calValue = 1024;
    unsigned short ina219_config = 0b0011000010001111;
    writeINA219(INA219_REG_CALIBRATION, ina219_calValue);
    writeINA219(INA219_REG_CONFIG, ina219_config);
}

float read_ina219(){
    float ma = 0;
    signed short value = readINA219(INA219_REG_CURRENT);
    ma = value / 3.0;
    return ma;
}

// write 2 bytes
void writeINA219(int reg, int value){
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = value>>8;
    buf[2] = value&0xff;

    HAL_I2C_Master_Transmit(&hi2c2, INA219_ADDR<<1, buf, 3, 10);
}

// read 2 bytes
signed short readINA219(unsigned char reg){
    HAL_I2C_Master_Transmit(&hi2c2, INA219_ADDR<<1, &reg, 1, 10);
    uint8_t buffer[2];
    HAL_I2C_Master_Receive(&hi2c2, INA219_ADDR<<1, buffer, 2, 10);

    signed short value = (buffer[0]<<8)|buffer[1];
    return value;
}

void set_motor_pwm(int pwm){
    if (pwm > U_MAX) pwm = U_MAX;
    if (pwm < -U_MAX) pwm = -U_MAX;

    if (pwm > 0) {
        // forward
    	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2400 - pwm);
    	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2400);
    } else if (pwm < 0) {
        // reverse
    	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2400);
    	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2400 + pwm);
    } else {
        // off
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2400);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2400);
    }
}
