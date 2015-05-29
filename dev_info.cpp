#include "engine_interface.h"
#include "world.h"
#include "render.h"

using namespace std;

void Engine_Interface::render_dev_info(){
    string msg="";

    if(game.in_progress){
        msg+="Camera Position: "+Strings::num_to_string(game.camera.x)+","+Strings::num_to_string(game.camera.y)+"\n";
        msg+="Camera Size: "+Strings::num_to_string(game.camera.w/game.camera_zoom)+","+Strings::num_to_string(game.camera.h/game.camera_zoom)+"\n";
        msg+="Camera Zoom: "+Strings::num_to_string(game.camera_zoom)+"\n";
    }

    if(msg.length()>0){
        Bitmap_Font* font=engine_interface.get_font("small");
		
		double y=2.0;
        if(engine_interface.option_fps){
            y+=font->spacing_y;

            if(network.status!="off"){
                y+=font->spacing_y*2.0;
            }
        }

        render_rectangle(2.0,y,Strings::longest_line(msg)*font->spacing_x,Strings::newline_count(msg)*font->spacing_y,0.75,"ui_black");
        font->show(2.0,y,msg,"red");
    }
}
