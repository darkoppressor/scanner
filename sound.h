#ifndef sound_h
#define sound_h

#include "sound_data.h"

#include <vector>
#include <string>
#include <stdint.h>

#include <SDL_mixer.h>

class Sound{
private:

    void _play_sound(Sound_Data* ptr_sound,int volume);

public:

    std::vector<Sound_Data> sounds;

    std::vector<std::string> sound_names;

    void add_sound(const Custom_Sound& sound);
	void add_sound(std::string name,Mix_Chunk* chunk);
    void remove_sound(std::string name);

    void load_sounds();
    void unload_sounds();

    //Modifies volume by applying the appropriate audio option multipliers.
    int modify_sound_volume(int volume);

    int get_free_channel();

    //If coordinates are not given, the sound is played at maximum volume.
    //falloff determines the rate at which a positional sound falls off.
    //The higher falloff is, the more slowly sounds will fade out as the distance between the sound receiver and the sound source increases.
    //So a low falloff means the sound will quickly falloff.
    void play_sound(std::string sound_name);
    void play_sound(std::string sound_name,double sound_x,double sound_y,double falloff=-1.0);
	
	//Stop all sounds
    void stop_sounds();

    Sound_Data* get_sound(std::string sound_name);
};

#endif
