#pragma once

UENUM(BlueprintType)
enum class EPowerUpsTypes : uint8
{
	Ept_Health			UMETA(DisplayName="Health"),
	Ept_Shield			UMETA(DisplayName="Shield"),
	Ept_Speed			UMETA(DisplayName="Speed"),
	Ept_FrozenStar		UMETA(DisplayName="Frozen Star"),
	Ept_Compass			UMETA(DisplayName="Compass")
};

namespace HealthProperties
{
	constexpr inline float BonusHealth {500.f };
}

namespace FrozenStarProperties
{
	constexpr inline float PauseDuration {15.f};;
}