// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/Ability.h"
#include "MeleeAbility.generated.h"

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UMeleeAbility : public UAbility
{
	GENERATED_BODY()

	public:

	UMeleeAbility();


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TSubclassOf<UDamageType> DamageType;
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float AttackDamageMultiplier = 1.f;

	

	//UAbility
	virtual void OnPostWaitForAnimationResource() override;	
	//aa/
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bCleaves;
	
	UFUNCTION()
	void Hitbox();
	UFUNCTION()
	void MontageEnded( UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY()
	UAnimMontage* ActiveMontage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundBase* HitSound;
	
protected:
	virtual void StartAbility() override;
	


	void PlayAttackAnimation();
	
	void TargetActor(AActor* Target);

	UPROPERTY()
	AActor* ActorTarget;
	

	
	virtual void DealEffects(struct FAbilityTargetData TargetData);

	
	
	
	
};
