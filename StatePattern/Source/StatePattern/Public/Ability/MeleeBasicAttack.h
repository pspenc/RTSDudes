// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/MeleeAbility.h"
#include "MeleeBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UMeleeBasicAttack : public UMeleeAbility
{
	GENERATED_BODY()

	public:
	
	UMeleeBasicAttack();
	virtual void NativeBeginPlay() override;
	
};
