#pragma once

#include <FNL/fast_noise_lite.h>

class NoiseGenerator
{
public:
	NoiseGenerator(NoiseGenerator& other) = delete;
	void operator=(const NoiseGenerator&) = delete;

	static NoiseGenerator& getInstance(int seed, float frequency,
		FastNoiseLite::NoiseType noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin,
		FastNoiseLite::FractalType fractalType = FastNoiseLite::FractalType::FractalType_FBm,
		int fractalOctaves = 4);

	static float getNoise2D(float x, float y);

private:
	NoiseGenerator();

	static FastNoiseLite generator;
};
