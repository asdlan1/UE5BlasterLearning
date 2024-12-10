// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "BlasterLearing/Character/BlasterCharacter.h"
#include "BlasterLearing/BlasterComponent/BufferComponent.h"

AHealthPickup::AHealthPickup()
{
	bReplicates = true;
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UBufferComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff)
		{
			Buff->Heal(HealthAmount, HealthingTime);
		}
	}
	Destroy();
}
