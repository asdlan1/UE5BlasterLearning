# UE5BlasterLearning

```cpp
//UFUNCTION(NetMulticast, Reliable)
//void MulticastAmmo(int32 UpdateAmmo);

UFUNCTION(Client, Reliable)
void ClientSyncAmmo(int32 UpdateAmmo);

/*
*我之前修改武器弹药异常这个Bug的时候使用的是上面的多播RPC去纠正客户端的武器弹药，但是实际上真正需要纠正弹药的只有玩家控制的这台客户端，所以这条多播RPC可以进行优化，没必要再去发送到其余的客户端造成无意义的网络资源耗费。
*所以我修改多播RPC为指定客户端RPC，在装备时服务器只会向玩家控制的这台客户端进行同步纠正弹药。但是注意这一步需要在设置完武器的所有者之后执行，也就是修改Combatponent.cpp的代码如下。删除装备武器方法当中的多播RPC调用。在装备主副武器的方法中添加指定客户端RPC的调用。
*/

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccuiped) return;
  //WeaponToEquip->MulticastAmmo(WeaponToEquip->GetAmmo());
  ...
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(WeaponToEquip);
	PlayEquipWeaponSound(WeaponToEquip);
	SecondaryWeapon->SetOwner(Character);
	WeaponToEquip->ClientSyncAmmo(WeaponToEquip->GetAmmo());
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetOwner(Character);
	WeaponToEquip->ClientSyncAmmo(WeaponToEquip->GetAmmo());
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(WeaponToEquip);
	ReloadEmptyWeapon();
}
```
