
/*
 * Another World engine rewrite
 * Copyright (C) 2004-2005 Gregory Montoir (cyx@users.sourceforge.net)
 */

#include <SDL.h>
#define MIX_INIT_FLUIDSYNTH MIX_INIT_MID // renamed with SDL2_mixer >= 2.0.2
#include <SDL_mixer.h>
#include <map>
#include "aifcplayer.h"
#include "mixer.h"
#include "sfxplayer.h"
#include "util.h"

enum {
	TAG_RIFF = 0x46464952,
	TAG_WAVE = 0x45564157,
	TAG_fmt  = 0x20746D66,
	TAG_data = 0x61746164
};

static const bool kAmigaStereoChannels = false; // 0,3:left 1,2:right

static int16_t toS16(int a) {
	return ((a << 8) | a) - 32768;
}

static int16_t mixS16(int sample1, int sample2) {
	const int sample = sample1 + sample2;
	return sample < -32768 ? -32768 : ((sample > 32767 ? 32767 : sample));
}

struct MixerChannel {
	const uint8_t *_data;
	Frac _pos;
	uint32_t _len;
	uint32_t _loopLen, _loopPos;
	int _volume;
	void (MixerChannel::*_mixWav)(int16_t *sample, int count);

	void initRaw(const uint8_t *data, int freq, int volume, int mixingFreq) {
		_data = data + 8;
		_pos.reset(freq, mixingFreq);

		const int len = READ_BE_UINT16(data) * 2;
		_loopLen = READ_BE_UINT16(data + 2) * 2;
		_loopPos = _loopLen ? len : 0;
		_len = len;

		_volume = volume;
	}

	void initWav(const uint8_t *data, int freq, int volume, int mixingFreq, int len, bool bits16, bool stereo, bool loop) {
	}

	void mixRaw(int16_t &sample) {
	}

	template<int bits, bool stereo>
	void mixWav(int16_t *samples, int count) {
	}
};

static const uint8_t *loadWav(const uint8_t *data, int &freq, int &len, bool &bits16, bool &stereo) {
	return data;
}

struct Mixer_impl {

	static const int kMixFreq = 44100;
	static const SDL_AudioFormat kMixFormat = AUDIO_S16SYS;
	static const int kMixSoundChannels = 2;
	static const int kMixBufSize = 4096;
	static const int kMixChannels = 4;

	Mix_Chunk *_sounds[kMixChannels];
	Mix_Music *_music;
	MixerChannel _channels[kMixChannels];
	SfxPlayer *_sfx;
	std::map<int, Mix_Chunk *> _preloads; // AIFF preloads (3DO)

	void init(MixerType mixerType) {
	}

	void quit() {
	}

	void update() {
	}

	void playSoundRaw(uint8_t channel, const uint8_t *data, int freq, uint8_t volume) {
	}

	void playSoundWav(uint8_t channel, const uint8_t *data, int freq, uint8_t volume, bool loop) {
	}

	void playSound(uint8_t channel, int volume, Mix_Chunk *chunk, int loops = 0) {
	}

	void stopSound(uint8_t channel) {
	}

	void freeSound(int channel) {
	}

	void setChannelVolume(uint8_t channel, uint8_t volume) {
	}

	void playMusic(const char *path, int loops = 0) {
	}

	void stopMusic() {
	}

	static void mixAifcPlayer(void *data, uint8_t *s16buf, int len) {
	}
	void playAifcMusic(AifcPlayer *aifc) {
	}
	void stopAifcMusic() {
	}

	void playSfxMusic(SfxPlayer *sfx) {
	}

	void mixChannels(int16_t *samples, int count) {
	}

	static void mixAudio(void *data, uint8_t *s16buf, int len) {
	}

	void mixChannelsWav(int16_t *samples, int count) {
	}

	static void mixAudioWav(void *data, uint8_t *s16buf, int len) {
	}

	void stopAll() {
	}

	void preloadSoundAiff(int num, const uint8_t *data) {
	}

	void playSoundAiff(int channel, int num, int volume) {
	}
};

Mixer::Mixer(SfxPlayer *sfx)
	: _aifc(0), _sfx(sfx) {
}

void Mixer::init(MixerType mixerType) {
}

void Mixer::quit() {
}

void Mixer::update() {
}

void Mixer::playSoundRaw(uint8_t channel, const uint8_t *data, uint16_t freq, uint8_t volume) {
}

void Mixer::playSoundWav(uint8_t channel, const uint8_t *data, uint16_t freq, uint8_t volume, uint8_t loop) {
}

void Mixer::stopSound(uint8_t channel) {
}

void Mixer::setChannelVolume(uint8_t channel, uint8_t volume) {
}

void Mixer::playMusic(const char *path, uint8_t loop) {
}

void Mixer::stopMusic() {
}

void Mixer::playAifcMusic(const char *path, uint32_t offset) {
}

void Mixer::stopAifcMusic() {
}

void Mixer::playSfxMusic(int num) {
}

void Mixer::stopSfxMusic() {
}

void Mixer::stopAll() {
}

void Mixer::preloadSoundAiff(uint8_t num, const uint8_t *data) {
}

void Mixer::playSoundAiff(uint8_t channel, uint8_t num, uint8_t volume) {
}
