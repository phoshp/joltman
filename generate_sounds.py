import wave
import struct
import random
import math
import os

SAMPLE_RATE = 44100

def write_wav(filename, samples):
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(SAMPLE_RATE)
        for s in samples:
            # clip
            s = max(-1.0, min(1.0, s))
            val = int(s * 32767.0)
            wav_file.writeframesraw(struct.pack('<h', val))

def generate_footstep():
    samples = []
    # Short noisy thump
    length = int(SAMPLE_RATE * 0.08)
    for i in range(length):
        env = 1.0 - (i / length)
        noise = random.uniform(-1.0, 1.0) * env
        low_freq = math.sin(2 * math.pi * 60 * i / SAMPLE_RATE) * env
        samples.append((noise * 0.15 + low_freq * 0.85) * 0.3)
    return samples

def generate_zombie_groan():
    samples = []
    length = int(SAMPLE_RATE * 0.8)
    for i in range(length):
        env = math.sin(math.pi * i / length)
        # Low freq saw wave with random noise modulation
        freq = 60 + math.sin(2 * math.pi * 3 * i / SAMPLE_RATE) * 15
        phase = (i * freq / SAMPLE_RATE) % 1.0
        wave_val = (phase * 2.0 - 1.0)
        noise = random.uniform(-0.3, 0.3)
        samples.append((wave_val * 0.6 + noise) * env * 0.4)
    return samples

def generate_hit():
    samples = []
    # Squishy or punchy hit
    length = int(SAMPLE_RATE * 0.15)
    for i in range(length):
        env = math.exp(-20.0 * i / length)
        freq = 300 * math.exp(-50.0 * i / length)
        wave_val = math.sin(2 * math.pi * freq * i / SAMPLE_RATE)
        noise = random.uniform(-1.0, 1.0)
        samples.append((wave_val * 0.5 + noise * 0.5) * env * 0.5)
    return samples

def generate_player_die():
    samples = []
    length = int(SAMPLE_RATE * 1.5)
    for i in range(length):
        env = 1.0 - (i / length)
        freq = 200 * env + 50
        wave_val = math.sin(2 * math.pi * freq * i / SAMPLE_RATE)
        noise = random.uniform(-1.0, 1.0) * (1.0 - env)
        samples.append((wave_val * 0.7 + noise * 0.3) * env * 0.6)
    return samples

def generate_pickup():
    samples = []
    length = int(SAMPLE_RATE * 0.3)
    for i in range(length):
        env = 1.0 - (i / length)
        freq = 400 + 800 * (i / length)
        wave_val = math.sin(2 * math.pi * freq * i / SAMPLE_RATE)
        samples.append(wave_val * env * 0.5)
    return samples

os.makedirs('assets/sounds', exist_ok=True)
write_wav('assets/sounds/step.wav', generate_footstep())
write_wav('assets/sounds/zombie.wav', generate_zombie_groan())
write_wav('assets/sounds/hit.wav', generate_hit())
write_wav('assets/sounds/die.wav', generate_player_die())
write_wav('assets/sounds/pickup.wav', generate_pickup())
print("Sounds generated successfully!")
