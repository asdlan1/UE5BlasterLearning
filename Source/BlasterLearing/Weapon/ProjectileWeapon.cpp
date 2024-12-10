// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	 const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	 UWorld* World = GetWorld();
	 if (MuzzleFlashSocket && World)
	 {
		 FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		 //目标位置减去枪口位置得来中间向量，再得到这个向量的方向作为子弹方向。
		 FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		 FRotator TargetRotation = ToTarget.Rotation();

		 FActorSpawnParameters SpawnParams;
		 SpawnParams.Owner = GetOwner();
		 SpawnParams.Instigator = InstigatorPawn;

		 AProjectile* SpawnedProjectile = nullptr;
		 if (bUseServerSideRewind)
		 {
			 if (InstigatorPawn->HasAuthority())//在服务器
			 {
				 if (InstigatorPawn->IsLocallyControlled())//服务器控制 生成复制子弹
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = false;
					 SpawnedProjectile->Damage = Damage;
					 SpawnedProjectile->HeadShotDamage = HeadShotDamage;
				 }
				 else//服务器但不是本地控制,生成不复制的子弹，使用倒带
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = true;
				 }
			 }
			 else//客户端使用倒带
			 {
				 if (InstigatorPawn->IsLocallyControlled())//客户端本地控制,生成不复制的子弹，使用倒带
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = true;
					 SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					 SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
					 //SpawnedProjectile->Damage = Damage;//其实写不写无所谓
				 }
				 else//客户端不是本地控制,生成一个非复制子弹不适用倒带
				 {
					 SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					 SpawnedProjectile->bUseServerSideRewind = false;
				 }
			 }
		 }
		 else//武器不使用倒带
		 {
			 if (InstigatorPawn->HasAuthority())
			 {
				 SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				 SpawnedProjectile->bUseServerSideRewind = false;
				 SpawnedProjectile->Damage = Damage;
				 SpawnedProjectile->HeadShotDamage = HeadShotDamage;
			 }
		 }
	 }
}
