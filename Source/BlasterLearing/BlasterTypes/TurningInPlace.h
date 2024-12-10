#pragma once

UENUM()
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Turning Left"),
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DsiplayName = "Not Turning"),

	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};