// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AI/UnitAIController.h"
#include "GameFramework/PlayerController.h"
#include "UnitPlayerController.generated.h"


class APlayerUnit;
class UUnitState;
class UAbility;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilitiesChanged);

/**
 * 
 */
UCLASS()
class STATEPATTERN_API AUnitPlayerController : public APlayerController
{
	GENERATED_BODY()



	public:

	AUnitPlayerController();
	
	UPROPERTY(BlueprintAssignable)
	FAbilitiesChanged OnAbilitiesChanged;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);
	
	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	void Turn(float Value);
	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	void LookUp(float Value);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector2D CameraYawBounds;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector2D CameraPitchBounds;
	
	
	void TryJump();
	void TryAttack();

	void UseAbility1();
	void UseAbility2();
	void UseAbility3();

	UFUNCTION(BlueprintCallable)
	void SwapToSpectatorMode();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void DisplayInterWaveHUD();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void DisplayGameHUD();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void StartWaveCountdown(float Time);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void ShowLoseScreen();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void ShowWinScreen();

	
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite)
	class UUI_GameHUD* GameHUD;
	UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite)
	class UUI_InterWave* InterWaveHUD;


	UFUNCTION(BlueprintCallable)
	void GiveChoosableRewards(TArray< URewardInstance*> Rewards);
	UPROPERTY(BlueprintReadOnly)
	TArray<URewardInstance*> ChoosableRewards;

	UFUNCTION(BlueprintCallable)
	void SelectUpgrade(URewardInstance* SelectedReward);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Ability1Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Ability2Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName Ability3Name;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Ability1Level;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Ability2Level;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Ability3Level;
	UPROPERTY(BlueprintReadOnly)
	UAbility* Ability1;
	UPROPERTY(BlueprintReadOnly)
	UAbility* Ability2;
	UPROPERTY(BlueprintReadOnly)
	UAbility* Ability3;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Reward|Data")
	UDataTable* AbilityToImpMappingTable;

	UFUNCTION(BlueprintCallable)
	void UseAbility(int Index);
	UFUNCTION(BlueprintPure)
	bool AbilitySlotEmpty(int Index) const;

	UFUNCTION(BlueprintCallable)
	int HasAbility(FName AbilityName);

	UFUNCTION()
	void GrantAbility(FName AbilityName, int AbilityLevel);
	UFUNCTION(BlueprintImplementableEvent)
	void PresentSlotSelection(FName NewAbility, int NewAbilityLevel);

	
	UFUNCTION(BlueprintCallable)
	void AssignSlot(FName AbilityName,int Level, int Slot);
	UFUNCTION(BlueprintCallable)
	void UpgradeAbility(FName AbilityName, int LevelsToIncreaseBy);
	
	UFUNCTION()
	void OnUnitHealthChanged(float NewHealth);

	UPROPERTY()
	UAbility* QueuedAbility;
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<class UUI_GameHUD> GameHUDClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<class UUI_InterWave> InterWaveHUDClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<class UUserWidget> LoseGameHUDClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<class UUserWidget> WinGameHUDClass;
	
	UPROPERTY(BlueprintReadOnly)
	APlayerUnit* ControlledUnit;

	
	
	
	
};


