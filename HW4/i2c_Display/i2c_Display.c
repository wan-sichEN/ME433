#include "i2cDisplay_lib.h"


int main()
{
    stdio_init_all();

    init_i2cDisplay();

    char message[50];
    sprintf(message, "Hello there");
    drawWord(0, 0, message);

    sprintf(message, "I am Optimus Prime"); 
    drawWord(0, 8, message);
    
    unsigned int t1 = to_us_since_boot(get_absolute_time());

    while (true) {
        printVoltage();
        unsigned int t2 = to_us_since_boot(get_absolute_time()); // the FPS for the first loop will be inaccurate bc t2 is calculated before printing, but following loops are correct
        float dt = (t2-t1)*1e-6; // converting us to s
        t1 = t2;
        sprintf(message, "FPS = %.3f", 1.0f/dt); // 1 frame in dt time
        drawWord(0, 24, message);
        ssd1306_update();
        
    }
}
