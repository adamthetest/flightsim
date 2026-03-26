#include "audio/AudioManager.h"
#include "core/AssetManager.h"
#include "raymath.h"

void AudioManager::Init(AssetManager& assets) {
    InitAudioDevice();
    SetMasterVolume(1.0f);
    m_initialized = true;
    (void)assets;
}

void AudioManager::Shutdown() {
    if (m_initialized) CloseAudioDevice();
}

void AudioManager::PlaySpatial(const std::string& soundKey, AssetManager& assets,
                               float distance, float maxDist) {
    if (!m_initialized) return;
    Sound& snd = assets.GetSound(soundKey);
    if (snd.stream.buffer == nullptr) return; // not loaded

    float vol = Clamp(1.0f - (distance / maxDist), 0.0f, 1.0f);
    vol *= m_sfxVolume;
    SetSoundVolume(snd, vol);
    PlaySound(snd);
}

void AudioManager::PlayUI(const std::string& soundKey, AssetManager& assets) {
    if (!m_initialized) return;
    Sound& snd = assets.GetSound(soundKey);
    if (snd.stream.buffer == nullptr) return;
    SetSoundVolume(snd, m_sfxVolume);
    PlaySound(snd);
}

void AudioManager::SetMusic(const std::string& musicKey, AssetManager& assets) {
    if (!m_initialized) return;
    Music& mus = assets.GetMusic(musicKey);
    if (mus.stream.buffer == nullptr) return;
    m_currentMusic = &mus;
    ::SetMusicVolume(*m_currentMusic, m_musicVolume);
    PlayMusicStream(*m_currentMusic);
}

void AudioManager::UpdateMusic() {
    if (!m_initialized || !m_currentMusic) return;
    if (m_currentMusic->stream.buffer) {
        UpdateMusicStream(*m_currentMusic);
    }
}

void AudioManager::SetMasterVolume(float v) {
    ::SetMasterVolume(Clamp(v, 0.0f, 1.0f));
}

void AudioManager::SetMusicVolume(float v) {
    m_musicVolume = Clamp(v, 0.0f, 1.0f);
    if (m_currentMusic && m_currentMusic->stream.buffer) {
        ::SetMusicVolume(*m_currentMusic, m_musicVolume);
    }
}

void AudioManager::SetSFXVolume(float v) {
    m_sfxVolume = Clamp(v, 0.0f, 1.0f);
}
