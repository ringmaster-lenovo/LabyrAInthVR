#pragma once

namespace WallSettings
{
	// How close are the walls from each other (distance from a 1 to another value in the matrix)
	inline constexpr float WallOffset{170.f};
	inline constexpr float WallHeight{100.f};
	inline constexpr int ShortWall{3};
	inline constexpr float FloorOffset{0.f}; // Horizontal distance from floor start to labyrinth
	inline constexpr float CeilingOffset{0.f}; // Horizontal distance from ceiling start to labyrinth
	inline uint8 LabyrinthWidth{11}; // Default value, assigned by LabyrinthParser upon receiving the matrix
	
	// Add your palettes here (RGB Values), in LabyrinthParser.h & ALabyrinthParser::GetRandomColor switch case
	inline std::vector<std::vector<float>> AquaPalette{
		{5.0f, 1.0f, 74.0f},
		{2.f, 0.f, 121.f},
		{0.f, 6.f, 177.f},
		{0.f, 19.f, 222.f},
		{0.f, 33.f, 243.f},
		{3.f, 4.f, 94.f},
		{2.f, 62.f, 138.f},
		{0.f, 119.f, 182.f},
		{0.f, 150.f, 199.f},
		{0.f, 180.f, 216.f},
		{72.f, 202.f, 228.f},
		{144.f, 224.f, 239.f},
		{173.f, 232.f, 244.f},
		{202.f, 240.f, 248.f}
	};
	
	inline std::vector<std::vector<float>> AutumnPalette{
		{117.f, 37.f, 10.f},
		{94.f, 8.f, 8.f},
		{80.f, 10.f, 9.f},
		{67.f, 12.f, 10.f},
		{42.f, 11.f, 9.f},
		{33.f, 15.f, 4.f},
		{69.f, 33.f, 3.f},
		{105.f, 5.f, 0.f},
		{147.f, 75.f, 0.f},
		{187.f, 107.f, 0.f},
		{248.f, 237.f, 235.f},
		{254.f, 200.f, 154.f},
		{255.f, 229.f, 217.f},
		{249.f, 220.f, 196.f}
	};

	inline std::vector<std::vector<float>> RosePalette{
		{37, 9, 2},
		{56, 4, 14},
		{100, 13, 20},
		{128, 14, 19},
		{173, 40, 49},
		{180, 60, 68},
		{187, 78, 85},
		{193, 94, 100},
		{199, 109, 114},
		{204, 122, 127},
	};

	inline std::vector<std::vector<float>> BluYellowPalette{
		{0, 31, 82},
		{0, 99, 204},
		{10, 133, 255},
		{71, 166, 255},
		{153, 206, 255},
		{215, 178, 29},
		{245, 188, 0},
		{255, 206, 10},
		{255, 218, 31},
		{255, 228, 92},
	};

	inline std::vector<std::vector<float>> ReddishPalette{
		{78, 1, 16},
		{136, 22, 0},
		{248, 112, 96},
		{243, 146, 55},
		{174, 163, 10},
		{192, 180, 12},
		{213, 203, 11},
		{240, 225, 0},
		{249, 243, 159},
		{253, 250, 217},
	};
}

namespace EnemySettings
{
	constexpr inline float SpawnHeight {125.f};
	constexpr inline float TraceLength {50'000.f};
	constexpr inline float TurnAtIntersecProbability {0.4f };
	constexpr inline float TurnAtDiagonalProbability {0.4f };
	constexpr inline bool bEnableLog {true};
}

namespace Misc
{
	constexpr inline float FacingEnemyDegrees {90.f };
}

namespace Interactables
{
	constexpr inline float PowerUpsSpawnHeight { 20.0f };
	constexpr inline float TrapsSpawnHeight { 0.0f };
}

namespace Weapons
{
	constexpr inline float SpawnHeight { 150.0f };
	constexpr inline float DistanceFromWall { 20.0f };
}
