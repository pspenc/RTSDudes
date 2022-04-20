// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Ability.h"
#include "Projectile/Projectile.h"

#include "ProjectileAbility.generated.h"


USTRUCT()
struct FRangedAttackConfig
{
	GENERATED_BODY()

	FRangedAttackConfig()
	{
		LaunchLocation = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		ProjectileClass = AProjectile::StaticClass();
		Magnitude = 1;
	}
	FRangedAttackConfig(FVector i_LaunchLocation,FRotator i_Rotation, TSubclassOf<AProjectile> i_ProjectileClass, float i_Magnitude = 1)
	{
		LaunchLocation = i_LaunchLocation;
		Rotation = i_Rotation;
		ProjectileClass = i_ProjectileClass;
		Magnitude = i_Magnitude;
	}
	
	UPROPERTY()
	FVector LaunchLocation;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	TSubclassOf<AProjectile> ProjectileClass;
	UPROPERTY()
	float Magnitude = 1;
};

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UProjectileAbility : public UAbility
{
	GENERATED_BODY()


	public:

	UProjectileAbility();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bUseUnitProjectileClass;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(EditCondition="!bUseUnitProjectileClass",EditConditionHides))
	TSubclassOf<class AProjectile> ProjectileClass;

	UFUNCTION()
	FRangedAttackConfig GetFiringInfo();

	UFUNCTION()
	void Release();


	//UAbility
	virtual void OnPostWaitForAnimationResource() override;
	UFUNCTION()
	void MontageEnded( UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY()
	UAnimMontage* ActiveMontage;

	protected:

	UPROPERTY()
	FRangedAttackConfig CachedAttackData;
	
	void PlayRangedAttackAnimation();
	

	//UAbility
	virtual void StartAbility() override;
	
};
