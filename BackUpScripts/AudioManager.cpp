//
//  AudioManager.cpp
//  GameEngine
//
//  Created by Chenxing Ouyang on 6/1/16.
//  Copyright © 2016 CSE167. All rights reserved.
//

#include "AudioManager.h"

Mix_Chunk * AudioManager::audio_1;
Mix_Chunk * AudioManager::audio_2;
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
    audio_1 = Mix_LoadWAV(AUDIO_1);
    if (audio_1 == NULL)
    {
        std::cerr << "Failed to load audio 1 error" << std::endl;
        return false;
    }
    
    // Load our 2st sound effect
    audio_2 = Mix_LoadWAV(AUDIO_2);
    if (audio_2 == NULL)
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


bool AudioManager::play_audio_1()
{
    if ( Mix_PlayChannel(-1, audio_1, 0) == -1 )
        return false;
    return true;
}

bool AudioManager::play_audio_2()
{
    if ( Mix_PlayChannel(-1, audio_2, 0) == -1 )
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
    Mix_FreeChunk(audio_1);
    Mix_FreeChunk(audio_2);
    Mix_FreeMusic(background_music);
    
    // quit SDL_mixer
    Mix_CloseAudio();
}



