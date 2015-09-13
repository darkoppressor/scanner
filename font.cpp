/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "font.h"
#include "render.h"
#include "world.h"

using namespace std;

Bitmap_Font::Bitmap_Font(){
    name="";

    spacing_x=0;
    spacing_y=0;

    gui_padding_x=0;
    gui_padding_y=0;

    shadow_distance=0;
}

void Bitmap_Font::build_font(){
    //Set the cell dimensions:
    double cell_width=sprite.get_width()/256.0;
    double cell_height=sprite.get_height();

    //The current character we are setting.
    int current_char=0;

    //Go through the cell columns.
    for(int cols=0;cols<256;cols++){
        //Set the character offset:
        chars[current_char].x=cell_width*cols;
        chars[current_char].y=0.0;

        //Set the dimensions of the character:
        chars[current_char].w=cell_width;
        chars[current_char].h=cell_height;

        //Go to the next character.
        current_char++;
    }
}

double Bitmap_Font::get_letter_width(){
    return sprite.get_width()/256.0;
}

double Bitmap_Font::get_letter_height(){
    return sprite.get_height();
}

void Bitmap_Font::show(double x,double y,string text,string font_color,double opacity,double scale_x,double scale_y,double angle,
                       SDL_Rect allowed_area,const vector<string>& character_colors){
    //Temporary offsets.
    double X=x,Y=y;

    double real_spacing_x=spacing_x*scale_x;
    double real_spacing_y=spacing_y*scale_y;

    //Go through the text.
    for(int show=0;text[show]!='\0';show++){
        //Get the ASCII value of the character.
        short ascii=(unsigned char)text[show];

        if(text[show]!='\xA'){
            if(X+get_letter_width()*scale_x>=0 && X<=main_window.SCREEN_WIDTH && Y+get_letter_height()*scale_y>=0 && Y<=main_window.SCREEN_HEIGHT){
                bool allowed_area_present=false;
                if(allowed_area.x!=-1 || allowed_area.y!=-1 || allowed_area.w!=0 || allowed_area.h!=0){
                    allowed_area_present=true;
                }

                if(!allowed_area_present || (allowed_area_present && X>=allowed_area.x && X+get_letter_width()*scale_x<=allowed_area.x+allowed_area.w && Y>=allowed_area.y && Y+get_letter_height()*scale_y<=allowed_area.y+allowed_area.h)){
                    if(shadow_distance!=0 && engine_interface.option_font_shadows){
                        //Render the shadow.
                        render_sprite(X+shadow_distance,Y+shadow_distance,*image.get_image(sprite.name),&chars[ascii],opacity,scale_x,scale_y,angle,"ui_black");
                    }

                    string character_color=font_color;

                    if(character_colors.size()==text.length() && character_colors[show].length()>0){
                        character_color=character_colors[show];
                    }

                    //Render the character.
                    render_sprite(X,Y,*image.get_image(sprite.name),&chars[ascii],opacity,scale_x,scale_y,angle,character_color);
                }
            }

            //Move over the width of the character with one pixel of padding.
            X+=real_spacing_x;
        }
        else{
            Y+=real_spacing_y;
            X=x;
        }
    }
}
