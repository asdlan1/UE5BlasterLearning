#pragma once

UENUM()
enum class ECombatState : uint8
{
	ECS_Unoccuiped UMETA(DsiplayName = "Unoccpied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_ThrowingGrenade UMETA(DisplayName = "Throwing Grenade"),
	ECS_SwappingWeapon UMETA(DisplayName = "Swapping Weapon"),

	ECS_MAX UMETA(DsiplayName = "DefaultMax")
};