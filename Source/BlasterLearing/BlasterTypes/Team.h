#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_RedTeam UMETA(DsiplayName = "RedTeam"),
	ET_BlueTeam UMETA(DsiplayName = "BlueTeam"),
	ET_NoTeam UMETA(DsiplayName = "NoTeam"),

	ET_MAX UMETA(DsiplayName = "DefaultMAX")
};