/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "button_events.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

bool Button_Events::handle_button_event(string button_event,Window* parent_window){
    bool window_opened_on_top=false;

    if(button_event.length()>0){
        if(button_event=="close_window"){
            if(parent_window!=0){
                parent_window->toggle_on(true,false);
            }
        }
        else if(boost::algorithm::starts_with(button_event,"open_window_")){
            boost::algorithm::erase_first(button_event,"open_window_");

            engine_interface.get_window(button_event)->toggle_on(true,true);
            window_opened_on_top=true;
        }
        else if(button_event=="quit"){
            engine_interface.quit();
        }
        else if(button_event=="configure_commands"){
            engine_interface.get_window("configure_commands")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="reset_default_commands"){
            engine_interface.game_commands.clear();
            engine_interface.load_data("game_command");
            engine_interface.save_game_commands();
        }
        else if(button_event=="show_data_location"){
            string save_path=engine_interface.get_save_directory_absolute();

            vector<string> lines;
            int spacing_x=engine_interface.get_font("small")->spacing_x;
            int window_width=(int)floor((double)main_window.SCREEN_WIDTH*0.8);

            while(save_path.length()*spacing_x>window_width){
                int i=window_width/spacing_x;
                lines.push_back(string(save_path,0,i));
                save_path.erase(save_path.begin(),save_path.begin()+i);
            }

            if(save_path.length()>0){
                lines.push_back(save_path);
            }

            save_path="";

            for(int i=0;i<lines.size();i++){
                save_path+=lines[i];

                if(i<lines.size()-1){
                    save_path+="\n";
                }
            }

            engine_interface.make_notice("Your save data is located here:\n\n"+save_path);
            window_opened_on_top=true;
        }
        else if(button_event=="start_game_window"){
            engine_interface.get_window("start_game")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="start_game"){
            engine_interface.close_all_windows();

            game.stop();

            game.start();
        }
        else if(button_event=="stop_game"){
            engine_interface.close_all_windows();

            game.stop();

            engine_interface.get_window("main_menu")->toggle_on();
        }

        else if(button_event=="options_window"){
            Window* window=engine_interface.get_window("options");

            window->set_info_text(0,engine_interface.get_option_value("cl_effect_limit"));
            window->set_info_tooltip(0,engine_interface.get_option_description("cl_effect_limit"));

            window->set_stated_button_state_index(0,Strings::string_to_bool(engine_interface.get_option_value("cl_screen_shake")));
            window->set_stated_button_tooltip(0,engine_interface.get_option_description("cl_screen_shake"));

            window->set_stated_button_state_index(1,Strings::string_to_bool(engine_interface.get_option_value("cl_rumble")));
            window->set_stated_button_tooltip(1,engine_interface.get_option_description("cl_rumble"));

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="options" || button_event=="options_apply"){
            if(parent_window!=0){
                engine_interface.apply_options(parent_window->get_info_text(0),
                                               Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(0))),
                                               Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(1))));

                if(button_event=="options"){
                    handle_button_event("close_window",parent_window);
                }
            }
        }
        else if(button_event=="options_graphics_window"){
            Window* window=engine_interface.get_window("options_graphics");

            window->set_info_text(0,engine_interface.get_option_value("cl_screen_width"));
            window->set_info_tooltip(0,engine_interface.get_option_description("cl_screen_width"));

            window->set_info_text(1,engine_interface.get_option_value("cl_screen_height"));
            window->set_info_tooltip(1,engine_interface.get_option_description("cl_screen_height"));

            window->set_info_text(2,engine_interface.get_option_value("cl_display_number"));
            window->set_info_tooltip(2,engine_interface.get_option_description("cl_display_number"));

            window->set_stated_button_state_index(0,Strings::string_to_bool(engine_interface.get_option_value("cl_fullscreen_state")));
            window->set_stated_button_tooltip(0,engine_interface.get_option_description("cl_fullscreen_state"));

            window->set_stated_button_state_index(1,Strings::string_to_bool(engine_interface.get_option_value("cl_vsync")));
            window->set_stated_button_tooltip(1,engine_interface.get_option_description("cl_vsync"));

            uint32_t fs_mode=0;
            if(engine_interface.get_option_value("cl_fullscreen_mode")=="desktop"){
                fs_mode=1;
            }
            else if(engine_interface.get_option_value("cl_fullscreen_mode")=="windowed"){
                fs_mode=2;
            }
            window->set_stated_button_state_index(2,fs_mode);
            window->set_stated_button_tooltip(2,engine_interface.get_option_description("cl_fullscreen_mode"));

            window->set_stated_button_state_index(3,Strings::string_to_bool(engine_interface.get_option_value("cl_fps")));
            window->set_stated_button_tooltip(3,engine_interface.get_option_description("cl_fps"));

            window->set_stated_button_state_index(4,Strings::string_to_bool(engine_interface.get_option_value("cl_hw_cursor")));
            window->set_stated_button_tooltip(4,engine_interface.get_option_description("cl_hw_cursor"));

            window->set_stated_button_state_index(5,Strings::string_to_bool(engine_interface.get_option_value("cl_font_shadows")));
            window->set_stated_button_tooltip(5,engine_interface.get_option_description("cl_font_shadows"));

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="options_graphics" || button_event=="options_graphics_apply"){
            if(parent_window!=0){
                engine_interface.apply_options_graphics(parent_window->get_info_text(0),parent_window->get_info_text(1),parent_window->get_info_text(2),
                                                        Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(0))),
                                                        Strings::lower_case(parent_window->get_stated_button_state(2)),
                                                        Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(1))),
                                                        Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(3))),
                                                        Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(4))),
                                                        Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(5))));

                if(button_event=="options_graphics"){
                    handle_button_event("close_window",parent_window);
                }
            }
        }
        else if(button_event=="options_audio_window"){
            Window* window=engine_interface.get_window("options_audio");

            double volume_global=Strings::string_to_double(engine_interface.get_option_value("cl_volume_global"));
            uint32_t n_volume_global=2;
            if(volume_global<=0.25){
                n_volume_global=0;
            }
            else if(volume_global<=0.5){
                n_volume_global=1;
            }
            window->set_stated_button_state_index(0,n_volume_global);
            window->set_stated_button_tooltip(0,engine_interface.get_option_description("cl_volume_global"));

            window->set_stated_button_state_index(1,Strings::string_to_bool(engine_interface.get_option_value("cl_mute_global")));
            window->set_stated_button_tooltip(1,engine_interface.get_option_description("cl_mute_global"));

            double volume_sound=Strings::string_to_double(engine_interface.get_option_value("cl_volume_sound"));
            uint32_t n_volume_sound=2;
            if(volume_sound<=0.25){
                n_volume_sound=0;
            }
            else if(volume_sound<=0.5){
                n_volume_sound=1;
            }
            window->set_stated_button_state_index(2,n_volume_sound);
            window->set_stated_button_tooltip(2,engine_interface.get_option_description("cl_volume_sound"));

            window->set_stated_button_state_index(3,Strings::string_to_bool(engine_interface.get_option_value("cl_mute_sound")));
            window->set_stated_button_tooltip(3,engine_interface.get_option_description("cl_mute_sound"));

            double volume_music=Strings::string_to_double(engine_interface.get_option_value("cl_volume_music"));
            uint32_t n_volume_music=2;
            if(volume_music<=0.25){
                n_volume_music=0;
            }
            else if(volume_music<=0.5){
                n_volume_music=1;
            }
            window->set_stated_button_state_index(4,n_volume_music);
            window->set_stated_button_tooltip(4,engine_interface.get_option_description("cl_volume_music"));

            window->set_stated_button_state_index(5,Strings::string_to_bool(engine_interface.get_option_value("cl_mute_music")));
            window->set_stated_button_tooltip(5,engine_interface.get_option_description("cl_mute_music"));

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="options_audio" || button_event=="options_audio_apply"){
            if(parent_window!=0){
                string mute_global="false";
                if(parent_window->get_stated_button_state(1)=="Muted"){
                    mute_global="true";
                }

                string mute_sound="false";
                if(parent_window->get_stated_button_state(3)=="Muted"){
                    mute_sound="true";
                }

                string mute_music="false";
                if(parent_window->get_stated_button_state(5)=="Muted"){
                    mute_music="true";
                }

                engine_interface.apply_options_audio(parent_window->get_stated_button_state(0),mute_global,parent_window->get_stated_button_state(2),mute_sound,parent_window->get_stated_button_state(4),mute_music);

                if(button_event=="options_audio"){
                    handle_button_event("close_window",parent_window);
                }
            }
        }
        else if(button_event=="options_input_window"){
            Window* window=engine_interface.get_window("options_input");

            window->set_stated_button_state_index(0,Strings::string_to_bool(engine_interface.get_option_value("cl_bind_cursor")));
            window->set_stated_button_tooltip(0,engine_interface.get_option_description("cl_bind_cursor"));

            window->set_stated_button_state_index(1,Strings::string_to_bool(engine_interface.get_option_value("cl_screen_keyboard")));
            window->set_stated_button_tooltip(1,engine_interface.get_option_description("cl_screen_keyboard"));

            window->set_stated_button_state_index(2,Strings::string_to_bool(engine_interface.get_option_value("cl_accelerometer_controller")));
            window->set_stated_button_tooltip(2,engine_interface.get_option_description("cl_accelerometer_controller"));

            window->set_stated_button_state_index(3,Strings::string_to_bool(engine_interface.get_option_value("cl_touch_controller_state")));
            window->set_stated_button_tooltip(3,engine_interface.get_option_description("cl_touch_controller_state"));

            double tc_opacity=Strings::string_to_double(engine_interface.get_option_value("cl_touch_controller_opacity"));
            uint32_t n_tc_opacity=2;
            if(tc_opacity<=0.25){
                n_tc_opacity=0;
            }
            else if(tc_opacity<=0.5){
                n_tc_opacity=1;
            }
            window->set_stated_button_state_index(4,n_tc_opacity);
            window->set_stated_button_tooltip(4,engine_interface.get_option_description("cl_touch_controller_opacity"));

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="options_input" || button_event=="options_input_apply"){
            if(parent_window!=0){
                engine_interface.apply_options_input(Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(0))),
                                                     Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(1))),
                                                     Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(2))),
                                                     Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(3))),
                                                     parent_window->get_stated_button_state(4));

                if(button_event=="options_input"){
                    handle_button_event("close_window",parent_window);
                }
            }
        }
        else if(button_event=="options_network_window"){
            Window* window=engine_interface.get_window("options_network");

            window->set_info_text(0,engine_interface.get_option_value("cl_name"));
            window->set_info_tooltip(0,engine_interface.get_option_description("cl_name"));

            window->set_stated_button_state_index(0,Strings::string_to_bool(engine_interface.get_option_value("cl_chat_timestamps")));
            window->set_stated_button_tooltip(0,engine_interface.get_option_description("cl_chat_timestamps"));

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="options_network" || button_event=="options_network_apply"){
            if(parent_window!=0){
                engine_interface.apply_options_network(parent_window->get_info_text(0),
                                               Strings::bool_to_string(Strings::string_to_bool(parent_window->get_stated_button_state(0))));

                if(button_event=="options_network"){
                    handle_button_event("close_window",parent_window);
                }
            }
        }

        else if(button_event=="start_server"){
            engine_interface.close_all_windows();

            Window* window=engine_interface.get_window("start_server");

            engine_interface.change_option("sv_network_name",window->get_info_text(0));
            engine_interface.change_option("sv_network_password",window->get_info_text(1));

            game.stop();

            game.start();

            if(!network.start_as_server()){
                game.stop();
            }
        }
        else if(button_event=="start_server_window"){
            Window* window=engine_interface.get_window("start_server");

            window->set_info_text(0,engine_interface.get_option_value("sv_network_name"));
            window->set_info_text(1,engine_interface.get_option_value("sv_network_password"));

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="server_list_window"){
            network.refresh_server_list();

            engine_interface.get_window("server_list")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="server_list_delete_window"){
            network.refresh_server_list();

            engine_interface.get_window("server_list_delete")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="server_list_edit_window"){
            network.refresh_server_list();

            engine_interface.get_window("server_list_edit")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="lan_server_list_window"){
            network.lan_refresh();

            engine_interface.get_window("lan_server_list")->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="add_server_window"){
            Window* window=engine_interface.get_window("add_server");

            if(window->get_info_text(2).length()==0){
                window->set_info_text(2,Strings::num_to_string(network.port));
            }

            window->toggle_on();
            window_opened_on_top=true;
        }
        else if(button_event=="add_server"){
            if(parent_window!=0){
                string password=parent_window->get_info_text(3);
                bool password_required=false;
                if(password.length()>0){
                    password_required=true;
                }
                network.add_server(parent_window->get_info_text(0),parent_window->get_info_text(1),Strings::string_to_unsigned_long(parent_window->get_info_text(2)),&password,password_required,0,0,"",0);

                handle_button_event("close_window",parent_window);
            }
        }
        else if(button_event=="edit_server"){
            if(parent_window!=0){
                network.edit_server(engine_interface.editing_server,parent_window->get_info_text(0),parent_window->get_info_text(1),Strings::string_to_unsigned_long(parent_window->get_info_text(2)),parent_window->get_info_text(3));

                engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
                engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
                engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();

                handle_button_event("close_window",parent_window);
            }
        }

        else if(boost::algorithm::starts_with(button_event,"configure_commands_")){
            boost::algorithm::erase_first(button_event,"configure_commands_");

            engine_interface.configure_command=Strings::string_to_long(button_event);
            engine_interface.get_window("configure_command")->toggle_on(true,true);
            window_opened_on_top=true;
        }

        else if(boost::algorithm::starts_with(button_event,"server_list_delete_")){
            boost::algorithm::erase_first(button_event,"server_list_delete_");

            network.remove_server(Strings::string_to_long(button_event));

            engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
            engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
            engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();
        }
        else if(boost::algorithm::starts_with(button_event,"server_list_edit_")){
            boost::algorithm::erase_first(button_event,"server_list_edit_");

            Window* window=engine_interface.get_window("edit_server");
            Server* server=network.get_server(Strings::string_to_long(button_event));

            if(server!=0){
                window->set_info_text(0,server->name);
                window->set_info_text(1,server->address);
                window->set_info_text(2,Strings::num_to_string(server->port));
                window->set_info_text(3,server->password);

                engine_interface.editing_server=Strings::string_to_long(button_event);

                window->toggle_on(true,true);
                window_opened_on_top=true;
            }
        }
        else if(boost::algorithm::starts_with(button_event,"server_list_")){
            boost::algorithm::erase_first(button_event,"server_list_");

            int server_index=Strings::string_to_long(button_event);

            Server* server=network.get_server(server_index);

            if(server!=0){
                //If the server requires a password but we don't have one saved
                if(server->password_required && server->password.length()==0){
                    network.server_list_connecting_index=server_index;

                    engine_interface.get_window("input_server_password")->toggle_on(true,true);
                    window_opened_on_top=true;
                }
                else{
                    engine_interface.close_all_windows();

                    game.stop();

                    network.set_server_target(server_index);

                    if(!network.start_as_client()){
                        game.stop();
                    }
                }
            }
        }
        else if(button_event=="refresh_server_list"){
            network.refresh_server_list();
        }
        else if(button_event=="lan_refresh"){
            network.lan_refresh();
        }
        else if(button_event=="lan_refresh_quick"){
            network.lan_refresh_quick();
        }
        else if(boost::algorithm::starts_with(button_event,"lan_server_list_save_")){
            boost::algorithm::erase_first(button_event,"lan_server_list_save_");

            Server* server=network.get_lan_server(Strings::string_to_long(button_event));

            if(server!=0){
                bool server_added=network.add_server(server->name,server->address,server->port,0,server->password_required,server->slots_filled,server->slots_total,server->version,server->ping);

                if(server_added){
                    engine_interface.make_toast("Server added to list");
                }
                else{
                    engine_interface.make_toast("Server updated on list");
                }
            }
        }
        else if(boost::algorithm::starts_with(button_event,"lan_server_list_")){
            boost::algorithm::erase_first(button_event,"lan_server_list_");

            int lan_server_index=Strings::string_to_long(button_event);

            Server* server=network.get_lan_server(lan_server_index);

            if(server!=0){
                if(server->password_required){
                    network.lan_connecting_index=lan_server_index;

                    engine_interface.get_window("input_server_password")->toggle_on(true,true);
                    window_opened_on_top=true;
                }
                else{
                    engine_interface.close_all_windows();

                    game.stop();

                    network.set_lan_server_target(lan_server_index);

                    if(!network.start_as_client()){
                        game.stop();
                    }
                }
            }
        }
        else if(button_event=="server_password"){
            engine_interface.close_all_windows();

            int lan_server_index=network.lan_connecting_index;
            int server_list_server_index=network.server_list_connecting_index;

            game.stop();

            Window* window=engine_interface.get_window("input_server_password");

            if(lan_server_index!=-1){
                network.set_lan_server_target(lan_server_index,window->get_info_text(0));
            }
            else{
                network.set_server_target(server_list_server_index,window->get_info_text(0));
            }

            window->set_info_text(0,"");

            if(!network.start_as_client()){
                game.stop();
            }
        }

        else{
            if(!handle_button_event_game(button_event,parent_window,window_opened_on_top)){
                Log::add_error("Invalid button event: '"+button_event+"'");
            }
        }
    }

    return window_opened_on_top;
}
