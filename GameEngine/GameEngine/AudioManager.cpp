//
//  AudioManager.cpp
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/1/16.
//  Copyright © 2016 CSE167. All rights reserved.
//

#include "AudioManager.h"

Mix_Chunk * AudioManager::roar;
Mix_Chunk * AudioManager::splat;
Mix_Music * AudioManager::background_music;

bool AudioManager::init()
{
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "SDL_Init error" << std::endl;
        return false;
    }
    
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        std::cerr << "Mix_OpenAudio error" << std::endl;
        return false;
    }
    
    // Load our 1st sound effect
    roar = Mix_LoadWAV(AUDIO_1);
    if (roar == NULL)
    {
        std::cerr << "Failed to load audio 1 error" << std::endl;
        return false;
    }
    
    // Load our 2st sound effect
    splat = Mix_LoadWAV(AUDIO_2);
    if (splat == NULL)
    {
        std::cerr << "Failed to load audio 2 error" << std::endl;
        return false;
    }
    
    // Load our music
    background_music = Mix_LoadMUS(MUS_PATH);
    if (background_music == NULL)
    {
        std::cerr << "Failed to load background music error" << std::endl;
        return false;
    }
    
    
    return true;
}


bool AudioManager::play_roar()
{
    if ( Mix_PlayChannel(-1, roar, 0) == -1 )
        return false;
    return true;
}

bool AudioManager::play_splat()
{
    if ( Mix_PlayChannel(-1, splat, 0) == -1 )
        return false;
    return true;
}

bool AudioManager::play_background_music()
{
    if ( Mix_PlayMusic( background_music, -1) == -1 )
        return false;
    return true;
}

void AudioManager::close()
{
    // clean up our resources
    Mix_FreeChunk(roar);
    Mix_FreeChunk(splat);
    Mix_FreeMusic(background_music);
    
    // quit SDL_mixer
    Mix_CloseAudio();
}



