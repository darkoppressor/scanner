/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "special_info.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

GPS_Altitude::GPS_Altitude(){
    interval=0.25;
    rows=721;
    columns=1440;
}

void GPS_Altitude::setup(){
    if(!is_setup()){
        File_IO_Binary_Load load("data/egm96/WW15MGH.DAC");

        if(load.file_loaded()){
            string egm96_data=load.get_data();

            for(size_t i=0;i<egm96_data.length();i+=2){
                if(i+1<egm96_data.length()){
                    int16_t number=(egm96_data[i]<<8)|(egm96_data[i+1]&0xff);

                    egm96_nums.push_back(number);
                }
            }
        }
    }
}

void GPS_Altitude::stop(){
    egm96_nums.clear();
}

bool GPS_Altitude::is_setup(){
    return egm96_nums.size()!=0;
}

double GPS_Altitude::get_offset(double gps_latitude,double gps_longitude){
    double latitude=gps_latitude;
    double longitude=gps_longitude>=0.0 ? gps_longitude : gps_longitude+360.0;

    int top_row=(int)((90.0-latitude)/interval);
    if(latitude<=-90.0){
        top_row=rows-2;
    }
    int bottom_row=top_row+1;

    int left_column=(int)(longitude/interval);
    int right_column=left_column+1;
    if(longitude>=360.0-interval){
        left_column=columns-1;
        right_column=0;
    }

    double latitude_top=90.0-top_row*interval;
    double longitude_left=left_column*interval;

    double ul=game.world.gps_altitude.get_post_offset(top_row,left_column);
    double ll=game.world.gps_altitude.get_post_offset(bottom_row,left_column);
    double lr=game.world.gps_altitude.get_post_offset(bottom_row,right_column);
    double ur=game.world.gps_altitude.get_post_offset(top_row,right_column);

    double u=(longitude-longitude_left)/interval;
    double v=(latitude_top-latitude)/interval;

    double pll=(1.0-u)*(1.0-v);
    double plr=u*(1.0-v);
    double pur=u*v;
    double pul=(1.0-u)*v;

    //meters
    return (pll*ll+plr*lr+pur*ur+pul*ul)/100.0;
}

double GPS_Altitude::get_post_offset(int row,int column){
    int k=row*columns+column;

    if(k<egm96_nums.size()){
        return egm96_nums[k];
    }
    else{
        return 0.0;
    }
}

string Special_Info::get_sensor_string(string sensor_name){
    string text="";

    if(android.get_sensor_availability(sensor_name)){
        if(android.get_sensor_state(sensor_name)){
            float sensor_values[SENSOR_VALUES_MAX];
            android.get_sensor_values(sensor_name,sensor_values);

            string sensor_value_labels[SENSOR_VALUES_MAX];
            android.get_sensor_value_labels(sensor_name,sensor_value_labels);

            int sensor_value_count=android.get_sensor_value_count(sensor_name);

            for(int i=0;i<sensor_value_count;i++){
                string label="";
                if(sensor_value_labels[i].length()>0){
                    label=sensor_value_labels[i]+": ";
                }

                text+=label+Strings::num_to_string(sensor_values[i])+" "+android.get_sensor_units(sensor_name);

                if(i<sensor_value_count-1){
                    text+="\n";
                }
            }
        }
        else{
            text+="Sensor offline";
        }
    }
    else{
        text+="Sensor unavailable";
    }

    return text;
}

string Special_Info::get_special_info_text(string special_info){
    string text="";

    if(special_info.length()>0){
        if(special_info=="configure_command"){
            if(engine_interface.configure_command!=-1 && engine_interface.configure_command<engine_interface.game_commands.size()){
                text+="Inputs currently bound to \""+engine_interface.game_commands[engine_interface.configure_command].title+"\":"+"\n\n";

                const char* ckey=SDL_GetScancodeName(engine_interface.game_commands[engine_interface.configure_command].key);
                const char* cbutton=SDL_GameControllerGetStringForButton(engine_interface.game_commands[engine_interface.configure_command].button);
                const char* caxis=SDL_GameControllerGetStringForAxis(engine_interface.game_commands[engine_interface.configure_command].axis);

                bool allow_keys_and_buttons=true;
                bool allow_axes=true;
                if(caxis!=0 && engine_interface.game_commands[engine_interface.configure_command].axis!=SDL_CONTROLLER_AXIS_INVALID){
                    allow_keys_and_buttons=false;
                }
                else{
                    allow_axes=false;
                }

                if(allow_keys_and_buttons){
                    text+="Keyboard Key: ";
                    if(ckey!=0 && engine_interface.game_commands[engine_interface.configure_command].key!=SDL_SCANCODE_UNKNOWN){
                        text+=Strings::first_letter_capital(ckey);
                    }
                    else{
                        text+="<NOT SET>";
                    }
                    text+="\n\n";

                    text+="Controller Button: ";
                    if(cbutton!=0 && engine_interface.game_commands[engine_interface.configure_command].button!=SDL_CONTROLLER_BUTTON_INVALID){
                        text+=Strings::first_letter_capital(cbutton);
                    }
                    else{
                        text+="<NOT SET>";
                    }
                    text+="\n\n";
                }

                if(allow_axes){
                    text+="Controller Axis: ";
                    if(caxis!=0 && engine_interface.game_commands[engine_interface.configure_command].axis!=SDL_CONTROLLER_AXIS_INVALID){
                        text+=Strings::first_letter_capital(caxis);
                    }
                    else{
                        text+="<NOT SET>";
                    }
                    text+="\n\n";
                }
            }
        }
        else if(special_info=="scan_acceleration"){
            text+=get_sensor_string("accelerometer");
            text+="\n\n";

            text+=get_sensor_string("gravity");
            text+="\n\n";

            text+=get_sensor_string("linear_acceleration");
            text+="\n\n";
        }
        else if(special_info=="scan_environment"){
            text+=get_sensor_string("ambient_temperature");
            text+="\n\n";

            text+=get_sensor_string("pressure");
            text+="\n\n";

            text+=get_sensor_string("light");
            text+="\n\n";

            text+=get_sensor_string("relative_humidity");
            text+="\n\n";

            if(android.get_sensor_availability("ambient_temperature") && android.get_sensor_state("ambient_temperature") &&
               android.get_sensor_availability("relative_humidity") && android.get_sensor_state("relative_humidity")){
                float sensor_values[SENSOR_VALUES_MAX];

                android.get_sensor_values("ambient_temperature",sensor_values);
                float ambient_temperature=sensor_values[0];

                android.get_sensor_values("relative_humidity",sensor_values);
                float relative_humidity=sensor_values[0];

                float m=17.62f;
                float tn=243.12f;

                float a=6.112f;
                float absolute_humidity=216.7f*(((relative_humidity/100.0f)*a*exp((m*ambient_temperature)/(tn+ambient_temperature)))/(273.15f+ambient_temperature));

                float numerator=log(relative_humidity/100.0f)+(m*ambient_temperature)/(tn+ambient_temperature);
                float dewpoint=tn*(numerator/(m-numerator));

                text+="Dew point: "+Strings::num_to_string(dewpoint)+" "+Symbols::degrees()+"C\n\n";

                text+="Absolute ambient humidity: "+Strings::num_to_string(absolute_humidity)+" g/m"+Symbols::cubed()+"\n\n";
            }
            else{
                text+="Dew point and absolute ambient humidity could not be calculated\n\n";
            }
        }
        else if(special_info=="scan_magnetic"){
            text+=get_sensor_string("magnetic_field");
            text+="\n\n";
        }
        else if(special_info=="scan_rotation"){
            text+=get_sensor_string("gyroscope");
            text+="\n\n";

            text+="Rotation vector (gyroscope):\n";
            text+=get_sensor_string("rotation_vector");
            text+="\n\n";

            text+="Geomagnetic rotation vector (magnetometer):\n";
            text+=get_sensor_string("geomagnetic_rotation_vector");
            text+="\n\n";
        }
        else if(special_info=="scan_other"){
            text+=get_sensor_string("proximity");
            text+="\n\n";

            text+=get_sensor_string("step_counter");
            text+="\n\n";
        }
        else if(special_info=="scan_position"){
            if(android.get_sensor_availability("pressure") && android.get_sensor_state("pressure")){
                float sensor_values[SENSOR_VALUES_MAX];

                android.get_sensor_values("pressure",sensor_values);
                //pascals
                float pressure=sensor_values[0]*100.0f;

                //pascals
                float base_pressure=101325.0f;
                //kelvin
                float base_temperature=288.15f;
                //meters/second^2
                float acceleration_due_to_gravity=9.80665f;
                //kelvin/meter
                float lapse_rate=-0.0065f;
                //joules/(kilogram*kelvin)
                float gas_constant_for_air=287.053f;
                //meters
                float altitude=(base_temperature/lapse_rate)*(pow((pressure/base_pressure),-lapse_rate*gas_constant_for_air/acceleration_due_to_gravity)-1.0f);

                text+="Altitude (calculated from pressure): "+Strings::num_to_string(altitude)+" m\n\n";
            }
            else{
                text+="Altitude could not be calculated from pressure\n\n";
            }

            if(android.get_gps_availability()){
                if(android.get_gps_accessibility()){
                    if(android.get_gps_state()){
                        Android_GPS gps_data=android.get_gps_readout();

                        if(game.world.gps_altitude.is_setup()){
                            double offset=game.world.gps_altitude.get_offset(gps_data.latitude,gps_data.longitude);

                            text+="Altitude (calculated from GPS): "+Strings::num_to_string(gps_data.altitude)+" "+Android_GPS::UNITS_ALTITUDE+"\n\n";
                            text+="Altitude (calculated from GPS, adjusted to MSL): "+Strings::num_to_string(gps_data.altitude+offset)+" "+Android_GPS::UNITS_ALTITUDE+"\n\n";
                        }
                        else{
                            text+="Altitude could not be calculated from GPS\n\n";
                        }

                        text+="Latitude: "+Strings::num_to_string(gps_data.latitude)+Android_GPS::UNITS_LATITUDE+"\n\n";
                        text+="Longitude: "+Strings::num_to_string(gps_data.longitude)+Android_GPS::UNITS_LONGITUDE+"\n\n";
                        text+="Bearing: "+Strings::num_to_string(gps_data.bearing)+Android_GPS::UNITS_BEARING+"\n\n";
                        text+="Speed: "+Strings::num_to_string(gps_data.speed)+" "+Android_GPS::UNITS_SPEED+"\n\n";
                        text+="GPS accuracy: "+Strings::num_to_string(gps_data.accuracy)+" "+Android_GPS::UNITS_ACCURACY+"\n\n";
                    }
                    else{
                        text+="GPS offline\n\n";
                    }
                }
                else{
                    text+="GPS inaccessible\n\n";
                }
            }
            else{
                text+="GPS unavailable\n\n";
            }
        }
        else{
            Log::add_error("Invalid special info text: '"+special_info+"'");
        }
    }

    return text;
}

string Special_Info::get_special_info_sprite(string special_info){
    string str_sprite_name="";

    if(special_info.length()>0){
        if(special_info=="example"){
            str_sprite_name="";
        }
        else{
            Log::add_error("Invalid special info sprite: '"+special_info+"'");
        }
    }

    return str_sprite_name;
}
