// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class STATEPATTERN_API UUI_GameHUD : public UUserWidget
{
	GENERATED_BODY()


	public:

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void StartWaveCountdown(float Time);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void EnemyDied();
	
};
