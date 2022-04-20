// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/CompositeDataTable.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameMode.h"
#include "Reward/RewardInstance.h"
#include "Reward/RewardTypes.h"
#include "StatePattern/StatePattern.h"
#include "Team/RTSTeamInterface.h"

#include "LaneTSGameMode.generated.h"


USTRUCT(BlueprintType)
struct FWaveUnitEntry 
{
	GENERATED_BODY()


public:


	FWaveUnitEntry()
	{
		UnitName = "Peasant";
		Amount = 1;
		TimeInBetween = 0;
		bAsync = true;
	}

	FWaveUnitEntry(FName InUnitName,int InAmount,float InTimeInBetween,bool InbAsync):
	UnitName(InUnitName),Amount(InAmount),TimeInBetween(InTimeInBetween),bAsync(InbAsync)
	{
	}
    
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName UnitName;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int Amount;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float TimeInBetween;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bAsync;
};

USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FWaveUnitEntry> UnitData;

    
    
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaveStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaveEnded);


/**
 * 
 */
UCLASS()
class STATEPATTERN_API ALaneTSGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:

	
	
	ALaneTSGameMode();

	UFUNCTION()
	void PlayerDied();

	
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintAssignable)
	FWaveStarted OnWaveStartedDispatch;
	UPROPERTY(BlueprintAssignable)
	FWaveEnded OnWaveEndedDispatch;
	UFUNCTION(BlueprintPure)
	int GetCurrentWave() const {return CurrentWave;}

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void StartWaveCountdown();
	
	UPROPERTY()
	TArray<FWaveData> CachedWaveData;
	UPROPERTY()
	bool bWaveActive;
	FWaveData CurrentWaveData;

	UPROPERTY()
	TArray<class AUnitSpawner*> RegisteredEnemySpawners;
	UPROPERTY()
	TArray<class AUnitSpawner*> RegisteredPlayerSpawners;
	UFUNCTION(BlueprintCallable)
	void RegisterSpawner(AUnitSpawner* Spawner,ETeam Team);
	UFUNCTION(BlueprintCallable)
	void UnRegisterSpawner(AUnitSpawner* Spawner, ETeam Team);
	//public team
	UFUNCTION(BlueprintPure)
	TArray<class AUnit*> GetUnitsOnTeam(ETeam Team);
	UFUNCTION(BlueprintPure)
	TArray<class AUnit*> GetUnitsOnOpposingteam(ETeam AskersTeam);
	UFUNCTION(BlueprintPure)
	int GetNumUnitsLeftOnTeam(ETeam Team) const;

	UFUNCTION(BlueprintCallable)
	void RemoveUnitFromTeam(AUnit* Unit);
	UFUNCTION(BlueprintCallable)
	void AddUnitToTeam(AUnit* Unit, ETeam Team);

	//end public team
	
	

	//public reward
	

	public:
	
	
	void ConfirmUpgrade(URewardInstance* RewardInstance);

	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<URewardInstance* > GetSelectedRewardHistory() const {return SelectedRewardHistory;}

	protected:
	
	UFUNCTION()
	float CalcTotalRewardWeighting();
	UFUNCTION()
	void PresentRewards();
	
	void GetRewards(TArray<URewardInstance*>&  Rewards, int Amount);
	bool bWaitingForPlayerToChooseReward;
	UPROPERTY()
	TArray<FRewardDataEntry> CachedRewardData;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Reward|Data")
	UCompositeDataTable* CombinedRewardDataTable;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Reward|Data")
	UDataTable* AbilityToImpMappingTable;

	void ApplyUnitUpgrade(FName StatName, int Factor);
	void ApplyPlayerUpgrade(FName StatName, int Factor);
	void ApplyPlayerAbilityReward(FName AbilityName, int Factor);
	UFUNCTION()
	void OnPlayerFinishedSelectingUpgrade();
	
	private:

	UPROPERTY()
	TArray<URewardInstance* > SelectedRewardHistory;

	
	//end of public reward
	
	protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Wave|Data")
	UDataTable* EnemyWaveSpawnsDataTable;


	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<FName,int> PersistantPlayerUnits;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<FName,int> PlayerUnitProduction;
	UFUNCTION(BlueprintCallable,Category="Units")
	void AddPersistantUnit(FName UnitName,int Amount = 1);
	UFUNCTION(BlueprintCallable,Category="Units")
	void RemovePersistantUnit(FName UnitName, int Amount = 1);
	UFUNCTION(BlueprintCallable,Category="Units")
	int GetNumPersistantUnits(FName UnitName);
	UFUNCTION(BlueprintPure,Category="Units")
	int GetUnitProduction(FName UnitName);
	UFUNCTION(BlueprintCallable,Category="Units")
	void AddUnitProduction(FName UnitName, int Amount);
	UFUNCTION(BlueprintCallable,Category="Units")
	void RemoveUnitProduction(FName UnitName, int Amount);

	UPROPERTY(BlueprintReadOnly,Transient)
	TArray<class AUnit*> PlayerUnits;
	UPROPERTY(BlueprintReadOnly,Transient)
	TArray<class AUnit*> EnemyUnits;
	
	UPROPERTY(Transient)
	TArray<AUnit*> PendingPlayerUnitRemovals;
	UPROPERTY(Transient)
	TArray<AUnit*> PendingEnemyUnitRemovals;
	
	virtual void Tick(float DeltaSeconds) override;
	 



	
	

		
	UPROPERTY(Transient,BlueprintReadOnly)
	bool bSpawningEnemies;
	UPROPERTY(Transient,BlueprintReadOnly)
	bool bSpawningPlayerUnits;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int FirstWave;
	UPROPERTY(Transient,BlueprintReadOnly)
	int CurrentWave;
	UPROPERTY(Transient,BlueprintReadOnly)
	int FinalWave;


	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void PreWave();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SpawnPlayersUnits();
	void PrepareEnemySpawns();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void StartWave();
	UFUNCTION(BlueprintCallable)
	void StartSpawningEnemies();

	bool CheckWaveComplete() const;

	UFUNCTION(BlueprintNativeEvent)
	void WaveComplete();
	
	UFUNCTION(BlueprintCallable)
	void ClearUnits();

	UFUNCTION(BlueprintNativeEvent)
	void PostWave();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void StartInterWavePhase();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void GoToWinScreen();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void GoToLoseScreen();
	
	//present rewards
	//player selects rewards

	



	
	
};

