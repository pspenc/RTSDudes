// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/BlendSpace1D.h"
#include "UnitAnimInstance.generated.h"


class UUnitState;
class AUnit;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathAnimComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDamagedAnimComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackPointReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackAnimFinished);

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()



public:

	virtual void NativeInitializeAnimation() override;
	UFUNCTION(BlueprintCallable)
	virtual void UpdateAnimation();

	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FDeathAnimComplete OnDeathAnimComplete;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FDamagedAnimComplete OnDamagedAnimComplete;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FAttackPointReached OnAttackPointReached;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FAttackAnimFinished OnAttackAnimFinished;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBlendSpace1D* MovementBlendSpace;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* IdleAnim;

	UPROPERTY(BlueprintReadOnly)
	float NormalizedSpeed;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bShouldPlayOnHitMontages;


	
protected:

	UPROPERTY(BlueprintReadOnly)
	AUnit* OwningUnit;

	UPROPERTY(BlueprintReadOnly)
	bool bCanMove;
	
};
