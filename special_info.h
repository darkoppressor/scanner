/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef special_info_h
#define special_info_h

#include <string>
#include <vector>

class GPS_Altitude{
public:

    std::vector<int16_t> egm96_nums;

    //degrees
    double interval;
    int rows;
    int columns;

    GPS_Altitude();

    void setup();
    void stop();

    bool is_setup();

    double get_offset(double gps_latitude,double gps_longitude);

    double get_post_offset(int row,int column);
};

class Special_Info{
public:

    std::string get_sensor_string(std::string sensor_name);

    std::string get_special_info_text(std::string special_info);

    std::string get_special_info_sprite(std::string special_info);
};

#endif
