// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/AOE_Ability.h"

#include "Ability/AbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Unit/Unit.h"

void UAOE_Ability::StartAbility()
{
	GrabActorsFromHitbox();
}

void UAOE_Ability::GrabActorsFromHitbox()
{

	APawn* OwnerPawn = Cast<APawn>(GetOwningActor());
	if(OwnerPawn)
	{
		FVector HitboxLocation = OwnerPawn->GetActorTransform().TransformPosition(HitboxOffset);
		TArray<FHitResult> Hits = GetTargetsInBox(HitboxLocation,BoxSize,OwnerPawn->GetActorRotation(),TeamTargetting);
		FAbilityTargetData TargetData;
		TargetData.AddHitResults(Hits);

		if(Particles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Particles,HitboxLocation,OwningUnit->GetActorRotation(),ParticleScale);
		}
		
		
		
		if(Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(),Sound,HitboxLocation);
		}
		ApplyAbilityEffects(TargetData);
		
	}
	EndAbility();
}

void UAOE_Ability::ApplyAbilityEffects(FAbilityTargetData TargetData)
{
	
	for(auto Hit :TargetData.HitResultTargets)
	{
		if(Hit.GetActor())
		{
		
			FRadialDamageEvent DamageEvent = FRadialDamageEvent();
			DamageEvent.Origin = Hit.TraceStart;
			
			DamageEvent.Params = RadialDamageParams;
			FString HitActorName = *Hit.GetActor()->GetName();
			float DealtDamage = Hit.GetActor()->TakeDamage(Damage,DamageEvent,
											OwningUnit->GetController(),OwningUnit);
			UE_LOG(LogUnitSystem,Display,TEXT("%s from %s: Dealt %d damage to %s"),*GetName(),*OwningUnit->GetPrettyName(),DealtDamage,*HitActorName);
			
			
		}
	}
}

