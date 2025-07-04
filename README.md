# UE5BlasterLearning



```cpp
//UFUNCTION(NetMulticast, Reliable)
//void MulticastAmmo(int32 UpdateAmmo);

UFUNCTION(Client, Reliable)
void ClientSyncAmmo(int32 UpdateAmmo);

/*
*我之前修改武器弹药异常这个Bug的时候使用的是上面的多播RPC去纠正客户端的武器弹药，但是实际上真正需要纠正弹药的只有玩家控制的这台客户端，
*所以这条多播RPC可以进行优化，没必要再去发送到其余的客户端造成无意义的网络资源耗费。
*所以我修改多播RPC为指定客户端RPC，在装备时服务器只会向玩家控制的这台客户端进行同步纠正弹药。
*但是注意这一步需要在设置完武器的所有者之后执行，也就是修改CombatComponent.cpp的代码如下。删除装备武器方法当中的多播RPC调用。在装备主副武器的方法中添加指定客户端RPC的调用。
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
	...
}

//如果有人在后续的测试的时候发现，我敲这个爆头伤害的判定不对啊，怎么打他后背也是爆头，打他侧面也是爆头。反正不从正面打他都是爆头。
//其实实际是因为，作者在设计的时候，倒带判断伤害的时候，先计算的头部HitBox，把其他部位的HitBox设置无碰撞，这就导致其实实际上的判定是穿过了外部的HitBox直接打中头部了。
//解决办法有两种，一种是修改头部HitBox的大小不和其他的重叠。还有一种是修改代码，把判定顺序修改一下，根据射击的角度来进行先判定其他部位还是再判定头部，这样实际打中其他部位的子弹就不会再计算为爆头了。代码如下。
//但还有个简单方法就是分别使用身体和头部的射线通道来进行判断，尤其是对于霰弹枪的伤害判定，如果使用角度，对一发喷子的8发弹丸需要做八次角度判定，而且还得是对这次伤害击中的角色数组中的所有角色进行循环想想就逆天。
//我是修改了喷子的爆头倍率，这样收益就不是很高，但是又能做到伤害几乎一样近距离一枪死的情况。
void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController)
		{
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
			{
				const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;

				UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
			}
			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (HitCharacter)
			{
				FVector CharacterForward = HitCharacter->GetActorForwardVector();
				FVector CharacterCenter = HitCharacter->GetActorLocation();
				FVector HitLocation = Hit.Location;
				CharacterForward.Z = 0.f;
				CharacterCenter.Z = 0.f;
				HitLocation.Z = 0.f;
				FVector TargetDirection = (CharacterCenter - HitLocation).GetSafeNormal();
				float DotProduct = FVector::DotProduct(CharacterForward, TargetDirection);
				float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
				bool bHitFront = true;
				if (Angle <= 60.f) bHitFront = false;

				if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled())
				{
					OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
						HitCharacter,
						TraceStart,
						InitialVelocity,
						OwnerController->GetServerTime() - OwnerController->SingleTripTime,
						bHitFront
					);
				}
			}
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
```
#### 文档文件是我学习后对蓝图和代码分别最后做的一些总结，蓝图有点乱，代码从头到尾都按照我的理解进行了注释。如果大家不介意我的肤浅理解和语言表达能力，有需要可以看一看，如果有帮到你理解，我也不胜荣幸哈哈，不过我肯定无法保证理解的全都正确，Sorry Sorrry。
