// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Ability.h"
#include "AOE_Ability.generated.h"

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UAOE_Ability : public UAbility
{
	GENERATED_BODY()
	
	public:
	

	virtual void StartAbility() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UParticleSystem* Particles;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector ParticleScale = FVector::OneVector;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	USoundBase* Sound;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Damage = 10;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRadialDamageParams RadialDamageParams;
	UFUNCTION(BlueprintCallable)
	void GrabActorsFromHitbox();
	
	//local space offset from casting character for the hitbox to fire off at
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector HitboxOffset;


	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector BoxSize;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	EAbilityTeamTargettingType TeamTargetting;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UParticleSystem* OnCastParticle;

	UFUNCTION()
	virtual void ApplyAbilityEffects(struct FAbilityTargetData TargetData);
};
