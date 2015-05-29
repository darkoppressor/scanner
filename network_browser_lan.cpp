#include "network.h"
#include "world.h"

#include <boost/algorithm/string.hpp>

using namespace std;

void Network::set_lan_server_target(int index,string get_password){
    if(index<lan_server_list.size()){
        server_address=lan_server_list[index].address;
        server_port=lan_server_list[index].port;
        server_password=get_password;
    }
}

void Network::add_lan_server(string get_name,string get_address,unsigned short get_port,bool password_required,uint32_t slots_filled,uint32_t slots_total,string version,int ping){
    int existing_index=-1;
    for(int i=0;i<lan_server_list.size();i++){
        if(lan_server_list[i].matches(get_address,get_port)){
            existing_index=i;
            break;
        }
    }

    //This server is not already in the list
    if(existing_index==-1){
        lan_server_list.push_back(Server());
        lan_server_list[lan_server_list.size()-1].name=get_name;
        lan_server_list[lan_server_list.size()-1].address=get_address;
        lan_server_list[lan_server_list.size()-1].port=get_port;
        lan_server_list[lan_server_list.size()-1].password_required=password_required;
        lan_server_list[lan_server_list.size()-1].slots_filled=slots_filled;
        lan_server_list[lan_server_list.size()-1].slots_total=slots_total;
        lan_server_list[lan_server_list.size()-1].version=version;
        lan_server_list[lan_server_list.size()-1].ping=ping;
    }
    //This server is already in the list
    else{
        lan_server_list[existing_index].name=get_name;
        lan_server_list[existing_index].password_required=password_required;
        lan_server_list[existing_index].slots_filled=slots_filled;
        lan_server_list[existing_index].slots_total=slots_total;
        lan_server_list[existing_index].version=version;
        lan_server_list[existing_index].ping=ping;
    }

    engine_interface.get_window("lan_server_list")->rebuild_scrolling_buttons();
}

Server* Network::get_lan_server(int index){
    Server* ptr_server=0;

    if(index<lan_server_list.size()){
        ptr_server=&lan_server_list[index];
    }

    return ptr_server;
}

void Network::lan_refresh(){
    if(status=="off"){
        start_as_server(false);
    }

    lan_server_list.clear();

    peer->Ping("255.255.255.255",port,false);

    engine_interface.get_window("lan_server_list")->rebuild_scrolling_buttons();
}

void Network::lan_refresh_quick(){
    if(status=="off"){
        start_as_server(false);
    }

    for(int i=0;i<lan_server_list.size();i++){
        peer->Ping(lan_server_list[i].address.c_str(),lan_server_list[i].port,false);
    }
}

void Network::receive_server_browser_info(){
    RakNet::BitStream bitstream(packet->data,packet->length,false);
    stat_counter_bytes_received+=bitstream.GetNumberOfBytesUsed();

    RakNet::MessageID type_id;
    bitstream.Read(type_id);

    RakNet::TimeMS timestamp;
    bitstream.Read(timestamp);

    unsigned int data_length=packet->length-sizeof(RakNet::MessageID)-sizeof(RakNet::TimeMS);

    string str_other_data="";
    for(unsigned int i=0;i<data_length;i++){
        unsigned char character;
        bitstream.Read(character);
        str_other_data+=character;
    }

    int other_data_fields=6;
    vector<string> other_data;
    boost::algorithm::split(other_data,str_other_data,boost::algorithm::is_any_of("@"));
    //Merge any extra strings into the server name string
    for(int i=other_data_fields;i<other_data.size();i++){
        other_data[other_data_fields-1]+="@"+other_data[i];
        other_data.erase(other_data.begin()+i);
        i--;
    }

    string str_connection_data=packet->systemAddress.ToString(true);
    vector<string> connection_data;
    boost::algorithm::split(connection_data,str_connection_data,boost::algorithm::is_any_of("|"));

    if(connection_data.size()>=2 && other_data.size()>=other_data_fields){
        string connection_address=connection_data[0];
        unsigned short connection_port=Strings::string_to_unsigned_long(connection_data[1]);

        bool connection_password_required=false;
        if(other_data[0]=="1"){
            connection_password_required=true;
        }

        uint32_t connection_slots_filled=Strings::string_to_unsigned_long(other_data[1]);
        uint32_t connection_slots_total=Strings::string_to_unsigned_long(other_data[2]);

        string connection_version=other_data[3];

        string connection_game_title=other_data[4];

        string connection_name=other_data[other_data_fields-1];

        int connection_ping=(int64_t)RakNet::GetTimeMS()-(int64_t)timestamp;

        //If this is a normal server that is accepting clients
        //and this server is not us
        //and the server is running the game we are playing
        if(connection_slots_total>1 && packet->guid!=id && connection_game_title==engine_interface.game_title){
            add_lan_server(connection_name,connection_address,connection_port,connection_password_required,connection_slots_filled,connection_slots_total,connection_version,connection_ping);

            //If this server is on the saved server list, update it
            for(int i=0;i<server_list.size();i++){
                if(server_list[i].matches(connection_address,connection_port)){
                    update_server(connection_address,connection_port,connection_password_required,connection_slots_filled,connection_slots_total,connection_version,connection_ping);

                    break;
                }
            }
        }
    }
    else{
        Log::add_log("Failed to read received server browser data: "+str_connection_data);
    }
}
