// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Reward/RewardInstance.h"
#include "Reward/RewardTypes.h"
#include "UI_RewardCard.generated.h"


/**
 * 
 */
UCLASS()
class STATEPATTERN_API UUI_RewardCard : public UUserWidget
{
	GENERATED_BODY()

	public:

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void SetRarityColor();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void SetData(URewardInstance* Data);

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void Fade();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void UnFade();
};
