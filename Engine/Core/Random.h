#pragma once
#include <random>
#include <cstdlib>

namespace nu {

	inline std::mt19937& Generator() {
		static std::random_device randomDevice;
		static std::mt19937 generator(randomDevice());

		return generator;
	}

	inline void SeedRandom(unsigned int seed) {
		Generator().seed(seed);
	}

	inline int RandomInt() {
		static std::uniform_int_distribution<> dist;

		return dist(Generator());
	}
	/// <summary>
	/// Get Random Integer between 0 and max (exclusive)
	/// </summary>
	/// <param name="max">Exclusive Max</param>
	/// <returns>Returns random number between 0 and max (exclusive)</returns>
	inline int RandomInt(int max) {
		std::uniform_int_distribution<> dist(0, max - 1);

		return dist(Generator());
	}

	/// <summary>
	/// Get Random Integer between min and max (exclusive)
	/// </summary>
	/// <param name="min">The smallest number it will return</param>
	/// <param name="max">Exclusive Max</param>
	/// <returns>Returns random number between min and max</returns>
	inline int RandomInt(int min, int max) {
		if (min > max) std::swap(min, max);
		std::uniform_int_distribution<> dist(min, max);

		return dist(Generator());
	}

	inline float RandomFloat() {
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

		return dist(Generator());
	}
	/// <summary>
	/// Get Random Float between 0 and max (exclusive)
	/// </summary>
	/// <param name="max">Exclusive Max</param>
	/// <returns>Returns a Random Float between 0 and max</returns>
	inline float RandomFloat(float max) {
		std::uniform_real_distribution<float> dist(0.0f, max);

		return dist(Generator());
	}

	/// <summary>
	/// Get Random Float between min and max (exclusive)
	/// </summary>
	/// <param name="min">The smallest number it will return</param>
	/// <param name="max">Exclusive Max</param>
	/// <returns>Returns a Random Float between min and max</returns>
	inline float RandomFloat(float min, float max) {
		if (min > max) std::swap(min, max);
		std::uniform_real_distribution<float> dist(min, max);
		
		return dist(Generator());
	}

	inline bool RandomBool() {
		std::bernoulli_distribution dist(0.5);

		return dist(Generator());
	}
}

