// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/MeleeAbility.h"
#include "Ability/AbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Unit/StateManager.h"
#include "Unit/Unit.h"
#include "Unit/UnitAnimInstance.h"


UMeleeAbility::UMeleeAbility()
{
	DamageType = UDamageType::StaticClass();
	bCleaves = false;
}


void UMeleeAbility::OnPostWaitForAnimationResource()
{
	Super::OnPostWaitForAnimationResource();
	PlayAttackAnimation();
}

void UMeleeAbility::Hitbox()
{
	if(!OwningUnit)
	{
		return;
	}
	
	UE_LOG(LogUnitSystem,Display,TEXT("%s : Creating hitbox for attack"),*GetName());
	FHitbox HitboxConfig = OwningUnit->GetDefaultMeleeHitbox();
	FAbilityTargetData TargetData;
	
	TargetData.AddHitResults(GetTargetsInHitbox(HitboxConfig,GetOwningActor()->GetActorRotation(),EAbilityTeamTargettingType::Enemy));
	DealEffects(TargetData);
}

void UMeleeAbility::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(!ActiveMontage){return;}
	if(Montage == ActiveMontage && bInterrupted)
	{
		UUnitAnimInstance* AnimInstance = Cast<UUnitAnimInstance>(OwningUnit->GetMesh()->GetAnimInstance());
		if(AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this,&UMeleeAbility::MontageEnded);
		}
		ActiveMontage = nullptr;
		//if our montage ended, we should also end prematurely
		EndAbility();
	}
}

void UMeleeAbility::DealEffects(FAbilityTargetData TargetData)
{
	if(TargetData.HitResultTargets.Num() > 0)
	{
		if(HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(),HitSound,OwningUnit->GetActorLocation());
		}
	}
	
	
	UE_LOG(LogUnitSystem,Display,TEXT("%s : Dealing damage to targets"),*GetName());
	for(auto Hit :TargetData.HitResultTargets)
	{
		if(Hit.GetActor())
		{
			FVector HitDir = UKismetMathLibrary::GetDirectionUnitVector(Hit.GetActor()->GetActorLocation(),Hit.ImpactPoint);

			FPointDamageEvent DamageEvent = FPointDamageEvent(GetOwningUnit()->GetAttackDamage() * AttackDamageMultiplier,
																Hit,HitDir,DamageType);
			FString HitActorName = *Hit.GetActor()->GetName();
			float DealtDamage = Hit.GetActor()->TakeDamage(GetOwningUnit()->GetAttackDamage()*AttackDamageMultiplier,DamageEvent,
											OwningUnit->GetController(),OwningUnit);
			UE_LOG(LogUnitSystem,Display,TEXT("%s from %s: Dealt %d damage to %s"),*GetName(),*OwningUnit->GetPrettyName(),DealtDamage,*HitActorName);
			//If we dont cleave, only hit the initial target. 
			if(!bCleaves){break;}
		}
	}
}








void UMeleeAbility::StartAbility()
{
	if(!OwningUnit)
	{
		EndAbility();
		return;
	}
	UE_LOG(LogUnitSystem,Display,TEXT("%s : Starting melee attack"),*GetName());
	AActor* PotentialTarget = OwningUnit->GetTarget();
	//if we have target, have target, interp to face and start attack animation
	if(PotentialTarget)
	{
		TargetActor(PotentialTarget);
		UE_LOG(LogUnitSystem,Display,TEXT("%s : Found target from owner, Target = %s"),*GetName(),*PotentialTarget->GetName());
	}

	PlayAttackAnimation();
	
	

	
}

void UMeleeAbility::PlayAttackAnimation()
{
	if(StateManager && TryClaimAnimationResource())
	{
		bWaitingForAnimationResource = false;
		//setup listeners
		UUnitAnimInstance* AnimInstance = Cast<UUnitAnimInstance>(OwningUnit->GetMesh()->GetAnimInstance());
		if(AnimInstance)
		{
			
			AnimInstance->OnAttackPointReached.AddUniqueDynamic(this,&UMeleeAbility::Hitbox);
			AnimInstance->OnAttackAnimFinished.AddUniqueDynamic(this,&UMeleeAbility::EndAbility);
			UE_LOG(LogUnitSystem,Display,TEXT("%s : Starting melee attack animation"),*GetName());
			//start montage	
			ActiveMontage = OwningUnit->PlayAttackMontage();
			if(ActiveMontage)
			{
				AnimInstance->OnMontageEnded.AddUniqueDynamic(this,&UMeleeAbility::MontageEnded);
			}
		}
		
		
	}
	else
	{
		bWaitingForAnimationResource = true;
	}
	
	
}

void UMeleeAbility::TargetActor(AActor* Target)
{
	ActorTarget = Target;
	if(OwningUnit)
	{
		if(StateManager && StateManager->MovementAllowed())
		{
			OwningUnit->FaceActor(ActorTarget);
		}
	}
}
