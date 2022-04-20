// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Reward/RewardTypes.h"

#include "UI_InterWave.generated.h"


/**
 * 
 */
UCLASS()
class STATEPATTERN_API UUI_InterWave : public UUserWidget
{
	GENERATED_BODY()



	public:

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void DisplayRewardSelection();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void HideRewardSelection();

};
