#include "i2cDisplay_lib.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

struct repeating_timer timer;
bool LED_error_state = false; // sends the pico LED to full on when true

void init_i2cDisplay(void){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Initializing pico's LED for heartbeat
    gpio_init(PICO_LED);
    gpio_set_dir(PICO_LED, GPIO_OUT);
    add_repeating_timer_ms(-1000, ledHeartbeat, NULL, &timer);

    // Initializing adc0 for FPS reading
    adc_init();
    adc_gpio_init(26);    
    adc_select_input(0);

    ssd1306_setup();
}

bool ledHeartbeat(struct repeating_timer *t){
    // error state (LED full on)
    if(LED_error_state){
        gpio_put(PICO_LED, 1);
        return true;
    }
    
    // normal state (LED blinking)
    static bool led_state = 1;
    gpio_put(PICO_LED, led_state);
    led_state = !led_state;
    return true;
}

void drawLetter(int x, int y, char letter){ // where x and y are leftmost and topmost pixels of the letter
    int letter_index = letter - 32; // first value is space, which is ASCII 32

    for(int col = 0; col < 5; col++){ // col being each pixel column that makes the letter
        char bits_on = ASCII[letter_index][col];

        for(int row = 0; row < 8; row++){
            char is_pixel_on = (bits_on >> row) & 1;
            ssd1306_drawPixel(x+col, y+row, is_pixel_on);
        }
    }
}

void drawWord(int x, int y, char *message){
    int i = 0;
    while(message[i] != '\0'){
        drawLetter(x+5*i, y, message[i]); //each letter should be 5 pixels away from the start of the previous letter
        i++;
    }
}

void printVoltage(void){
    uint16_t adc_value = adc_read();
    float voltage = adc_value*3.3f / 4095.0f;
    
    char message[50];
    sprintf(message, "Volt = %.3f", voltage);
    drawWord(0, 16, message);
}
