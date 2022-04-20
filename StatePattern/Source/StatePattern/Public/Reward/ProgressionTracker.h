// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProgressionTracker.generated.h"

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UProgressionTracker : public UObject
{
	GENERATED_BODY()
	
	public:

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int UnitAttackUpgrades;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int UnitDefenseUpgrades;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int UnitSpeedUpgrades;
	
	
};
