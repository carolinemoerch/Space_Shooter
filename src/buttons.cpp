#include "buttons.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <iostream>
#include <chrono>

/* Buttons Layout
 *                     0
 * 5  4              3   2
 *                     1
 */

/* GPIOs
 *                     12
 * 27  22            5    6
 *                     16
 */

 /* Buttons are ACTIVE LOW */

 // Button pins
#define BTN_LEFT   5
#define BTN_RIGHT  6
#define BTN_UP     12
#define BTN_DOWN   16
#define BTN_A      27
#define BTN_B      22

Buttons::Buttons(GameState *game): game(game)
{
    chip_fd = open("/dev/gpiochip0",O_RDONLY);
    if(chip_fd < 0){
        perror("open()");
    }

    memset(&req_in,0,sizeof(req_in));
    req_in.lineoffsets[0] = BTN_UP;
    req_in.lineoffsets[1] = BTN_DOWN;
    req_in.lineoffsets[2] = BTN_LEFT;
    req_in.lineoffsets[3] = BTN_RIGHT;
    req_in.lineoffsets[4] = BTN_A;
    req_in.lineoffsets[5] = BTN_B;

    req_in.flags = GPIOHANDLE_REQUEST_INPUT;

    req_in.lines = 6;

    ret_val = ioctl(chip_fd,GPIO_GET_LINEHANDLE_IOCTL,&req_in);
    if(ret_val < 0){
        perror("ioctl(1)");
    }
}

Buttons::~Buttons()
{
     if (req_in.fd >= 0)
        close(req_in.fd);
    if (chip_fd >= 0)
        close(chip_fd);
}

Vec2 Buttons::readMoveButtons()
{
    /* TODO: ReadMoveButtons
     * returns latest value of move buttons:
     * - up: {0, -1}
     * - down {0, 1}
     * - left: {-1 ,0}
     * - right: {1, 0}
     * - None pressed: {0, 0}   
     */
    if(data_in.values[0] == 0)
        return {0,-1};
    if(data_in.values[1] == 0)
        return {0,1};
    if(data_in.values[2] == 0)
        return {-1,0};
    if(data_in.values[3] == 0)
        return {1,0};


    return {0, 0};
}

class EdgeDetector{
private:
    int lastValue = 1;
public:
    EdgeDetector(int initialValue) : lastValue(initialValue) {};
    bool valueChanged(int value)
    {
        bool falling = (value != lastValue && value == 0);
        lastValue = value;
        return falling;
    }
};

void Buttons::buttonsThread()
{
    int lastLeft = 1;
    int lastRight = 1;
    while (game->running)
    {
        // Fixed speed
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        /* TODO: Buttons Thread
         * 1) Run loop at fixed update speed @ 25 ms
         * 2) Update GPIO input data buffer
         * 3) Notify if shoot button has changed
         */
        readGPIOs();
        buttonLeft = data_in.values[4];
        buttonRight = data_in.values[5];
    
        if(buttonLeft == 0 && lastLeft == 1){
            game->gameMutex.lock();
                game->shootButtonLeft_flag = true;
            game->gameMutex.unlock();
            game->shootButton_cv.notify_one();
        }
         if(buttonRight == 0 && lastRight == 1){
            game->gameMutex.lock();
                game->shootButtonRight_flag = true;
            game->gameMutex.unlock();
            game->shootButton_cv.notify_one();
        }

        lastLeft = buttonLeft;
        lastRight = buttonRight;
    }
}

void Buttons::readGPIOs()
{
    /* TODO: Read GPIOs
     * 1) Update input data buffer with current GPIO state  
     */
    ret_val = ioctl(req_in.fd,GPIOHANDLE_GET_LINE_VALUES_IOCTL,&data_in);
    if(ret_val < 0){
        perror("ioctl(2)");
    }
}