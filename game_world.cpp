#include "game_world.h"
#include "world.h"
#include "render.h"

using namespace std;

Game_World::Game_World(){
    clear_world();
}

void Game_World::clear_world(){
    ///example_objects.clear();
}

void Game_World::generate_world(){
    clear_world();
}

void Game_World::tick(){
}

void Game_World::ai(){
}

void Game_World::movement(){
}

void Game_World::events(){
}

void Game_World::animate(){
}

void Game_World::render(){
}

void Game_World::update_background(){
}

void Game_World::render_background(){
    render_rectangle(0,0,main_window.SCREEN_WIDTH,main_window.SCREEN_HEIGHT,1.0,"ui_black");
}
