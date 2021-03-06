/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "network.h"
#include "world.h"

using namespace std;

void Network::set_server_target(string get_address,unsigned short get_port,string get_password){
    server_address=get_address;
    server_port=get_port;
    server_password=get_password;
}

void Network::set_server_target(int index,string get_password){
    if(index<server_list.size()){
        server_address=server_list[index].address;
        server_port=server_list[index].port;
        if(get_password.length()>0){
            server_password=get_password;
        }
        else{
            server_password=server_list[index].password;
        }
    }
}

bool Network::add_server(string get_name,string get_address,unsigned short get_port,const string* get_password,bool password_required,uint32_t slots_filled,uint32_t slots_total,string version,int ping){
    bool added=false;

    int existing_index=-1;
    for(int i=0;i<server_list.size();i++){
        if(server_list[i].matches(get_address,get_port)){
            existing_index=i;
            break;
        }
    }

    //This server is not already in the list
    if(existing_index==-1){
        added=true;

        server_list.push_back(Server());
        server_list[server_list.size()-1].name=get_name;
        server_list[server_list.size()-1].address=get_address;
        server_list[server_list.size()-1].port=get_port;
        if(get_password!=0){
            server_list[server_list.size()-1].password=*get_password;
        }
        server_list[server_list.size()-1].password_required=password_required;
        server_list[server_list.size()-1].slots_filled=slots_filled;
        server_list[server_list.size()-1].slots_total=slots_total;
        server_list[server_list.size()-1].version=version;
        server_list[server_list.size()-1].ping=ping;
    }
    //This server is already in the list
    else{
        server_list[existing_index].name=get_name;
        if(get_password!=0){
            server_list[existing_index].password=*get_password;
        }
        server_list[existing_index].password_required=password_required;
        server_list[existing_index].slots_filled=slots_filled;
        server_list[existing_index].slots_total=slots_total;
        server_list[existing_index].version=version;
        server_list[existing_index].ping=ping;
    }

    engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
    engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
    engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();

    engine_interface.save_servers();

    return added;
}

void Network::update_server(string get_address,unsigned short get_port,bool password_required,uint32_t slots_filled,uint32_t slots_total,string version,int ping){
    int existing_index=-1;
    for(int i=0;i<server_list.size();i++){
        if(server_list[i].matches(get_address,get_port)){
            existing_index=i;
            break;
        }
    }

    if(existing_index!=-1){
        server_list[existing_index].password_required=password_required;
        server_list[existing_index].slots_filled=slots_filled;
        server_list[existing_index].slots_total=slots_total;
        server_list[existing_index].version=version;
        server_list[existing_index].ping=ping;

        engine_interface.get_window("server_list")->rebuild_scrolling_buttons();
        engine_interface.get_window("server_list_delete")->rebuild_scrolling_buttons();
        engine_interface.get_window("server_list_edit")->rebuild_scrolling_buttons();
    }
}

void Network::remove_server(int index){
    if(index<server_list.size()){
        server_list.erase(server_list.begin()+index);

        engine_interface.save_servers();
    }
}

void Network::edit_server(int index,string get_name,string get_address,unsigned short get_port,string get_password){
    if(index<server_list.size()){
        Server* ptr_server=&server_list[index];

        ptr_server->name=get_name;
        ptr_server->address=get_address;
        ptr_server->port=get_port;
        ptr_server->password=get_password;

        engine_interface.save_servers();
    }
}

Server* Network::get_server(int index){
    Server* ptr_server=0;

    if(index<server_list.size()){
        ptr_server=&server_list[index];
    }

    return ptr_server;
}

void Network::refresh_server_list(){
    if(status=="off"){
        start_as_server(false);
    }

    for(int i=0;i<server_list.size();i++){
        peer->Ping(server_list[i].address.c_str(),server_list[i].port,false);
    }
}

bool Network::start_as_client(){
    if(status=="off"){
        peer=RakNet::RakPeerInterface::GetInstance();

        RakNet::SocketDescriptor sd;
        RakNet::StartupResult startup=peer->Startup(1,&sd,1);

        if(startup==RakNet::RAKNET_STARTED){
            status="client";

            id=peer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
            address=peer->GetSystemAddressFromGuid(id);

            connect_to_server();

            return true;
        }
        else{
            string error_message="Error initializing client: "+translate_startup_error(startup);

            Log::add_error(error_message);

            engine_interface.get_window("main_menu")->toggle_on("on","on");

            engine_interface.make_notice(error_message);
        }
    }

    return false;
}

void Network::connect_to_server(){
    if(status=="client"){
        if(server_password.length()==0){
            peer->Connect(server_address.c_str(),server_port,0,0);
        }
        else{
            peer->Connect(server_address.c_str(),server_port,server_password.c_str(),server_password.length());
        }

        Log::add_log("Attempting to connect to server: "+server_address+"|"+Strings::num_to_string(server_port));

        engine_interface.get_window("network_connecting")->toggle_on(true,true);
    }
}

void Network::receive_version(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    RakNet::RakString rstring;

    bitstream.ReadCompressed(rstring);
    string game_title=rstring.C_String();

    bitstream.ReadCompressed(rstring);
    string version=rstring.C_String();

    bitstream.ReadCompressed(rstring);
    string checksum=rstring.C_String();

    string our_game_title=engine_interface.game_title;
    if(game_title!=our_game_title){
        Log::add_log("Game mismatch: "+string(packet->systemAddress.ToString(true))+"\nOur game: "+our_game_title+"\nServer game: "+game_title);

        engine_interface.button_events_manager.handle_button_event("stop_game");
        engine_interface.make_notice("Server is running a different game.");
    }
    else{
        string our_version=engine_interface.get_version();
        if(version!=our_version){
            Log::add_log("Version mismatch: "+string(packet->systemAddress.ToString(true))+"\nOur version: "+our_version+"\nServer version: "+version);

            engine_interface.button_events_manager.handle_button_event("stop_game");
            engine_interface.make_notice("Version mismatch with server.");
        }
        else{
            if(checksum.length()>0 && checksum!=CHECKSUM){
                Log::add_log("Checksum mismatch: "+string(packet->systemAddress.ToString(true))+"\nOur checksum: "+CHECKSUM+"\nServer checksum: "+checksum);

                engine_interface.button_events_manager.handle_button_event("stop_game");
                engine_interface.make_notice("Checksum mismatch with server.");
            }
            else{
                engine_interface.get_window("network_connecting")->toggle_on(true,false);

                send_client_data(true);
            }
        }
    }
}

void Network::send_client_data(bool first_send){
    if(status=="client"){
        char ordering_channel=ORDERING_CHANNEL_CLIENT_DATA;
        if(first_send){
            ordering_channel=ORDERING_CHANNEL_CONNECTION;
        }

        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_CLIENT_DATA);

        bitstream.WriteCompressed(first_send);
        bitstream.WriteCompressed((RakNet::RakString)game.option_name.c_str());
        bitstream.WriteCompressed(rate_bytes);
        bitstream.WriteCompressed(rate_updates);

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,MEDIUM_PRIORITY,RELIABLE_ORDERED,ordering_channel,server_id,false);
    }
}

void Network::receive_initial_game_data(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    double update_rate=0.0;
    bitstream.Read(update_rate);

    engine_interface.set_logic_update_rate(update_rate);

    game.start_client();

    read_initial_game_data(&bitstream);

    timer_tick.start();

    send_connected();
}

void Network::send_connected(){
    if(status=="client"){
        RakNet::BitStream bitstream;
        bitstream.Write((RakNet::MessageID)ID_GAME_CONNECTED);

        stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
        peer->Send(&bitstream,MEDIUM_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_CONNECTION,server_id,false);
    }
}

void Network::receive_client_list(){
    clients.clear();

    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    int clients_size=0;
    bitstream.ReadCompressed(clients_size);
    for(int i=0;i<clients_size;i++){
        RakNet::RakString rstring;
        bitstream.ReadCompressed(rstring);

        clients.push_back(Client_Data(RakNet::UNASSIGNED_RAKNET_GUID,RakNet::UNASSIGNED_SYSTEM_ADDRESS,rstring.C_String(),false));
        clients[clients.size()-1].connected=true;
    }

    int client_index=0;
    bitstream.ReadCompressed(client_index);

    if(client_index>=0 && clients.size()>client_index){
        clients[client_index].is_us=true;
    }
}

void Network::receive_update(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID timestamp_id;
    bitstream.Read(timestamp_id);

    RakNet::Time timestamp;
    bitstream.Read(timestamp);

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    if(RakNet::LessThan(last_update_time,timestamp)){
        last_update_time=timestamp;

        read_update(&bitstream);
    }
}

void Network::send_input(){
    if(status=="client"){
        if(game.in_progress){
            if(commands_this_second<rate_commands && ++counter_commands>=(uint32_t)ceil(UPDATE_RATE/(double)rate_commands)){
                counter_commands=0;

                commands_this_second++;

                RakNet::BitStream bitstream;
                bitstream.Write((RakNet::MessageID)ID_GAME_INPUT);

                bitstream.WriteCompressed((int)command_buffer.size());
                for(int i=0;i<command_buffer.size();i++){
                    bitstream.WriteCompressed((RakNet::RakString)command_buffer[i].c_str());
                }
                command_buffer.clear();

                bitstream.WriteCompressed((int)game.command_states.size());
                for(int i=0;i<game.command_states.size();i++){
                    bitstream.WriteCompressed((RakNet::RakString)game.command_states[i].c_str());
                }

                stat_counter_bytes_sent+=bitstream.GetNumberOfBytesUsed();
                peer->Send(&bitstream,HIGH_PRIORITY,RELIABLE_ORDERED,ORDERING_CHANNEL_INPUT,server_id,false);
            }
        }
    }
}

void Network::receive_ping_list(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    int clients_size=0;
    bitstream.ReadCompressed(clients_size);
    for(int i=1;i<clients_size;i++){
        int ping=0;
        bitstream.ReadCompressed(ping);

        if(i<clients.size()){
            clients[i].ping=ping;
        }
    }

    if(clients.size()>0){
        clients[0].ping=peer->GetAveragePing(server_id);
    }
}

void Network::receive_paused(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    game.toggle_pause();
}

void Network::receive_sound(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    RakNet::RakString rstring;
    bitstream.ReadCompressed(rstring);

    sound_system.play_sound(rstring.C_String());
}
