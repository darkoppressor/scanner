#ifndef game_world_h
#define game_world_h

#include "special_info.h"

#include <vector>

class Game_World{
public:

    ///std::vector<Example> example_objects;

    GPS_Altitude gps_altitude;

    Game_World();

    void clear_world();
    void generate_world();

    void tick();
    void ai();
    void movement();
    void events();
    void animate();
    void render();

    void update_background();
    void render_background();
};

#endif
