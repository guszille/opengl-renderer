#include "noise_generator.h"

FastNoiseLite NoiseGenerator::generator;

NoiseGenerator::NoiseGenerator()
{
}

NoiseGenerator& NoiseGenerator::getInstance(int seed, float frequency, FastNoiseLite::NoiseType noiseType, FastNoiseLite::FractalType fractalType, int fractalOctaves)
{
	static NoiseGenerator singletonInstance;

	generator.SetSeed(seed);
	generator.SetFrequency(frequency);

	generator.SetNoiseType(noiseType);

	generator.SetFractalType(fractalType);
	generator.SetFractalOctaves(fractalOctaves);

	return singletonInstance;
}

float NoiseGenerator::getNoise2D(float x, float y)
{
	return generator.GetNoise(x, y);
}
