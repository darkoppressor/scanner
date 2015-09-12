/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "button_events.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

bool Button_Events::handle_button_event_game(string button_event,Window* parent_window,bool& window_opened_on_top){
    if(button_event=="scan_acceleration"){
        android.enable_sensor("accelerometer");
        android.enable_sensor("gravity");
        android.enable_sensor("linear_acceleration");

        engine_interface.get_window(button_event)->toggle_on(true,true);
        window_opened_on_top=true;

        return true;
    }
    else if(button_event=="scan_environment"){
        android.enable_sensor("ambient_temperature");
        android.enable_sensor("pressure");
        android.enable_sensor("relative_humidity");
        android.enable_sensor("light");

        engine_interface.get_window(button_event)->toggle_on(true,true);
        window_opened_on_top=true;

        return true;
    }
    else if(button_event=="scan_magnetic"){
        android.enable_sensor("magnetic_field");

        engine_interface.get_window(button_event)->toggle_on(true,true);
        window_opened_on_top=true;

        return true;
    }
    else if(button_event=="scan_rotation"){
        android.enable_sensor("gyroscope");
        android.enable_sensor("rotation_vector");
        android.enable_sensor("geomagnetic_rotation_vector");

        engine_interface.get_window(button_event)->toggle_on(true,true);
        window_opened_on_top=true;

        return true;
    }
    else if(button_event=="scan_other"){
        android.enable_sensor("proximity");
        android.enable_sensor("step_counter");

        engine_interface.get_window(button_event)->toggle_on(true,true);
        window_opened_on_top=true;

        return true;
    }
    else if(button_event=="scan_position"){
        android.enable_sensor("pressure");

        android.enable_gps(2000,0.0f);

        game.world.gps_altitude.setup();

        engine_interface.get_window(button_event)->toggle_on(true,true);
        window_opened_on_top=true;

        return true;
    }

    return false;
}
