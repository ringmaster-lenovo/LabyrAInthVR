#include "PlayerStatsVisitor.h"
#include "LabyrAInthVR/Player/PlayerStatistics.h"

void UPlayerStatsVisitor::ModifyHealth(AMainCharacter* MainCharacter, float BonusHealth)
{
	if(BonusHealth && IsValid(MainCharacter))
	{
		UPlayerStatistics* PlayerStatistics=MainCharacter->GetPlayerStatistics();

		if(PlayerStatistics->IsValidLowLevel())
		{
			PlayerStatistics->ChangeStatFloat(Esm_Health,BonusHealth);
		}
	}
}

void UPlayerStatsVisitor::ModifyShield(AMainCharacter* MainCharacter, bool IsActive)
{
	if(IsValid(MainCharacter))
	{
		UPlayerStatistics* PlayerStatistics=MainCharacter->GetPlayerStatistics();

		if(PlayerStatistics->IsValidLowLevel())
		{
			PlayerStatistics->ChangeStatBool(Esm_Shield,IsActive);
		}
	}
}
