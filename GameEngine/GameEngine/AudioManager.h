//
//  AudioManager.h
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/1/16.
//  Copyright © 2016 CSE167. All rights reserved.
//

#ifndef AudioManager_h
#define AudioManager_h

#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_mixer/SDL_mixer.h>

#define AUDIO_1 "../../Audio/roar_2.mp3"
#define AUDIO_2 "../../Audio/death.wav"
#define AUDIO_3 "../../Audio/death_roar.mp3"
#define AUDIO_4 "../../Audio/dictator_speech.mp3"
#define MUS_PATH "../../Audio/Time_cut.mp3"

class AudioManager
{
private:
    static Mix_Chunk *roar;
    static Mix_Chunk *death;
    static Mix_Chunk *death_roar;
    static Mix_Chunk *dictator_speech;
    static Mix_Music *background_music;
    
public:
    // Return false if can not load files
    static bool init();
    static bool play_roar();
    static bool play_death();
    static bool play_death_roar();
    static bool play_dictator_speech();
    static bool play_background_music();
    // Clean up our resources
    static void close();
};


#endif /* AudioManager_h */
