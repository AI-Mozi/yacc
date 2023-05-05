#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "audio.h"

SDL_AudioSpec audio_spec;
SDL_AudioDeviceID audio_device;

void audio_callback(void* userdata, uint8_t *stream, int len) {
    int duration_ms = *(int*)userdata;
    static double phase = 0.0;
    
    for (int i = 0; i < len; i++) {
        stream[i] = (uint8_t)(128 + 64 * sin(phase));
        phase += 2 * M_PI * 440.0 / 44100;
    }
    SDL_Delay(duration_ms);
}

void initAudio() {
    int duration = 500;
    audio_spec.freq = 40100;
    audio_spec.format = AUDIO_U8;
    audio_spec.channels = 1;
    audio_spec.samples = 1024;
    audio_spec.callback = audio_callback;
    audio_spec.userdata = &duration;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
}

void playSound() {
    SDL_PauseAudioDevice(audio_device, 0);
    SDL_Delay(50);
    SDL_PauseAudioDevice(audio_device, 1);
}
