/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "window.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Window::exec_close_function(){
    if(close_function.length()>0){
        if(close_function=="configure_command"){
            engine_interface.configure_command=-1;
        }
        else if(close_function=="scan_acceleration"){
            android.disable_sensor("accelerometer");
            android.disable_sensor("gravity");
            android.disable_sensor("linear_acceleration");
        }
        else if(close_function=="scan_environment"){
            android.disable_sensor("ambient_temperature");
            android.disable_sensor("pressure");
            android.disable_sensor("relative_humidity");
            android.disable_sensor("light");
        }
        else if(close_function=="scan_magnetic"){
            android.disable_sensor("magnetic_field");
        }
        else if(close_function=="scan_rotation"){
            android.disable_sensor("gyroscope");
            android.disable_sensor("rotation_vector");
            android.disable_sensor("geomagnetic_rotation_vector");
        }
        else if(close_function=="scan_other"){
            android.disable_sensor("proximity");
            android.disable_sensor("step_counter");
        }
        else if(close_function=="scan_position"){
            android.disable_sensor("pressure");

            android.disable_gps();

            game.world.gps_altitude.stop();
        }
        else{
            Log::add_error("Invalid close function: '"+close_function+"'");
        }
    }
}
