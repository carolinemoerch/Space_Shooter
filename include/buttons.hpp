#pragma once
#include "game_types.hpp"
#include "game_state.hpp"
#include <linux/gpio.h>
#include <mutex>

class Buttons {
public:
    Buttons(GameState *game);
    ~Buttons();
    Vec2 readMoveButtons();
    void buttonsThread();

private:
    void readGPIOs();

private:
    GameState *game;
    int chip_fd = -1;
    struct gpiohandle_request req_in;
    struct gpiohandle_data data_in;
    int ret_val = 0;
    bool buttonLeft = 1;
    bool buttonRight = 1;
};
