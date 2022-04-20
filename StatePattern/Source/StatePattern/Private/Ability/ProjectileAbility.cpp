// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ProjectileAbility.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "StatePattern/StatePattern.h"
#include "Unit/StateManager.h"
#include "Unit/Unit.h"
#include "Unit/UnitAnimInstance.h"

UProjectileAbility::UProjectileAbility()
{
	bUseUnitProjectileClass = true;
}


void UProjectileAbility::OnPostWaitForAnimationResource()
{
	Super::OnPostWaitForAnimationResource();
	PlayRangedAttackAnimation();
}

void UProjectileAbility::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(!ActiveMontage){return;}
	if(ActiveMontage == Montage && bInterrupted)
	{
		UUnitAnimInstance* AnimInstance = Cast<UUnitAnimInstance>(OwningUnit->GetMesh()->GetAnimInstance());
		if(AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this,&UProjectileAbility::MontageEnded);
		}
		ActiveMontage = nullptr;
		EndAbility();
	}
}

FRangedAttackConfig UProjectileAbility::GetFiringInfo()
{
	FRotator LaunchRot;
	FVector ReleaseLoc = OwningUnit->GetActorTransform().TransformPosition(FVector(25,0,25));
	if(!OwningUnit->IsPlayerControlled()) //if AI
	{
		AActor* Target  = GetAbilityTarget();
		
		
		if(Target)
		{
			LaunchRot = UKismetMathLibrary::FindLookAtRotation(ReleaseLoc,Target->GetActorLocation());
		
		}
		else
		{
			LaunchRot = OwningUnit->GetActorRotation();
		}
		
		}
		else   //if player
		{
		
		FHitResult HitResult = OwningUnit->GetTargettingTrace();

		if(HitResult.bBlockingHit)
		{
			//we had a hit
			LaunchRot = UKismetMathLibrary::FindLookAtRotation(ReleaseLoc,HitResult.Location);
		
		}
		else
		{
			//we had no hit
			LaunchRot = UKismetMathLibrary::FindLookAtRotation(ReleaseLoc,HitResult.TraceEnd);

		}
		}
	
	LaunchRot.Pitch = FMath::Clamp(LaunchRot.Pitch,-10.f,60.f);
	FRotator FacingDir = FRotator(0,LaunchRot.Yaw,0);
	
	return FRangedAttackConfig(ReleaseLoc,FacingDir,
					bUseUnitProjectileClass ? OwningUnit->RangedAttackProjectile : ProjectileClass);
}

void UProjectileAbility::Release()
{

	
	
	
	CachedAttackData = GetFiringInfo();
	FireProjectile(CachedAttackData.ProjectileClass,CachedAttackData.LaunchLocation,CachedAttackData.Rotation,GetAbilityTarget());
}




void UProjectileAbility::PlayRangedAttackAnimation()
{
	if(StateManager && TryClaimAnimationResource())
	{
		
		//setup listeners
		UUnitAnimInstance* AnimInstance = Cast<UUnitAnimInstance>(OwningUnit->GetMesh()->GetAnimInstance());
		if(AnimInstance)
		{
			AnimInstance->OnAttackPointReached.AddUniqueDynamic(this,&UProjectileAbility::Release);
			AnimInstance->OnAttackAnimFinished.AddUniqueDynamic(this,&UProjectileAbility::EndAbility);
			//start montage
			UE_LOG(LogUnitSystem,Display,TEXT("%s : Playing ranged attack animation"),*GetName());
		
			ActiveMontage = OwningUnit->PlayAttackMontage();
			if(ActiveMontage)
			{
				
				AnimInstance->OnMontageEnded.AddUniqueDynamic(this,&UProjectileAbility::MontageEnded);
			}

		}
		
		
	}
	else
	{
		bWaitingForAnimationResource = true;
	}
}



void UProjectileAbility::StartAbility()
{

	
	if(!OwningUnit){return;}

	UE_LOG(LogUnitSystem,Display,TEXT("%s : Starting Projectile Attack"),*GetName());
	
	

	PlayRangedAttackAnimation();
	
	
	
}
