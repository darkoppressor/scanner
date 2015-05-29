#include "game_constants.h"
#include "world.h"

using namespace std;

double ZOOM_RATE=0.0;
double ZOOM_MIN=0.0;
double ZOOM_MAX=0.0;

void set_game_constant(string name,string value){
    if(name=="zoom_rate"){
        ZOOM_RATE=Strings::string_to_double(value);
    }
    else if(name=="zoom_min"){
        ZOOM_MIN=Strings::string_to_double(value);
    }
    else if(name=="zoom_max"){
        ZOOM_MAX=Strings::string_to_double(value);
    }
}
