// Fill out your copyright notice in the Description page of Project Settings.


#include "LaneTSGameMode.h"

#include "EngineUtils.h"
#include "UnitPlayerController.h"
#include "Ability/Ability.h"
#include "Kismet/GameplayStatics.h"
#include "Unit/Unit.h"
#include "Wave/UnitSpawner.h"


ALaneTSGameMode::ALaneTSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	FirstWave = 0;
}

void ALaneTSGameMode::PlayerDied()
{
	GoToLoseScreen();
}

void ALaneTSGameMode::PresentRewards()
{
	AUnitPlayerController* PC = Cast<AUnitPlayerController>(UGameplayStatics::GetPlayerController(this,0));
	if(PC)
	{
		bWaitingForPlayerToChooseReward = true;
		TArray<URewardInstance*> RandomRewards;
		GetRewards(RandomRewards,3);
		
		PC->GiveChoosableRewards(RandomRewards);
	}
}
void ALaneTSGameMode::ConfirmUpgrade(URewardInstance*  RewardInstance)
{
	bWaitingForPlayerToChooseReward = false;
	ERewardRarity Rarity = RewardInstance->Rarity;
	SelectedRewardHistory.Add(RewardInstance);
	switch(RewardInstance->Type)
	{
		case ERewardType::PersistantUnit:

			AddPersistantUnit(RewardInstance->RewardTarget,RewardInstance->Factor);
		PreWave();
		break;
		case ERewardType::UnitProduction:
			AddUnitProduction(RewardInstance->RewardTarget,RewardInstance->Factor);
		PreWave();
		break;
		case ERewardType::UnitUpgrade:
			ApplyUnitUpgrade(RewardInstance->RewardTarget,RewardInstance->Factor);
		PreWave();
		break;
		case ERewardType::PlayerUpgrade:
			ApplyPlayerUpgrade(RewardInstance->RewardTarget,RewardInstance->Factor);
		PreWave();
			break;
		case ERewardType::PlayerAbility:
			ApplyPlayerAbilityReward(RewardInstance->RewardTarget,RewardInstance->Factor);
		break;
	
	}
	
}

void ALaneTSGameMode::ApplyUnitUpgrade(FName StatName, int Factor)
{
	
}

void ALaneTSGameMode::ApplyPlayerUpgrade(FName StatName, int Factor)
{
	
}

void ALaneTSGameMode::ApplyPlayerAbilityReward(FName AbilityName, int Factor)
{
	
	AUnitPlayerController* PC = Cast<AUnitPlayerController>(UGameplayStatics::GetPlayerController(this,0));
	if(PC)
	{
		PC->OnAbilitiesChanged.AddUniqueDynamic(this,&ALaneTSGameMode::OnPlayerFinishedSelectingUpgrade);
		PC->GrantAbility(AbilityName,Factor);
	}
	
}

void ALaneTSGameMode::OnPlayerFinishedSelectingUpgrade()
{
	PreWave();
}


void ALaneTSGameMode::StartWaveCountdown_Implementation()
{
	AUnitPlayerController* PC = Cast<AUnitPlayerController>(UGameplayStatics::GetPlayerController(this,0));
	if(PC)
	{
		PC->StartWaveCountdown(3.f);
	}
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle,this,&ALaneTSGameMode::StartWave,3.f);
}

void ALaneTSGameMode::RegisterSpawner(AUnitSpawner* Spawner, ETeam Team)
{
	Team == ETeam::Enemy ?  RegisteredEnemySpawners.Add(Spawner) : RegisteredPlayerSpawners.Add(Spawner);
}

void ALaneTSGameMode::UnRegisterSpawner(AUnitSpawner* Spawner, ETeam Team)
{
	Team == ETeam::Enemy ?  RegisteredEnemySpawners.Remove(Spawner) : RegisteredPlayerSpawners.Remove(Spawner);
}

void ALaneTSGameMode::BeginPlay()
{
	Super::BeginPlay();
	CurrentWave = FirstWave;
	if(EnemyWaveSpawnsDataTable)
	{
		CachedWaveData.Empty();
		const FString ContextString;
		TArray<FWaveData*> Data;
		EnemyWaveSpawnsDataTable->GetAllRows(ContextString,Data);
		for(auto DataPtr : Data)
		{
			CachedWaveData.Add(*DataPtr);
		}
		FinalWave = CachedWaveData.Num();
	}
	if(CombinedRewardDataTable)
	{
		CachedRewardData.Empty();
		const FString ContextString;
		TArray<FRewardDataEntry*> Data;
		CombinedRewardDataTable->GetAllRows(ContextString,Data);
		for(auto DataPtr : Data)
		{
			CachedRewardData.Add(*DataPtr);
		}
		CachedRewardData.Sort();
	}
}

TArray<AUnit*> ALaneTSGameMode::GetUnitsOnTeam(ETeam Team)
{
	if(Team == ETeam::Enemy)
	{
		return EnemyUnits;
	}
	else if(Team == ETeam::Player)
	{
		return PlayerUnits;
	}
	return TArray<AUnit*>();
}

TArray<AUnit*> ALaneTSGameMode::GetUnitsOnOpposingteam(ETeam AskersTeam)
{
	if(AskersTeam == ETeam::Enemy)
	{
		return PlayerUnits;
	}
	else if(AskersTeam == ETeam::Player)
	{
		return EnemyUnits;
	}
	return TArray<AUnit*>();
}

int ALaneTSGameMode::GetNumUnitsLeftOnTeam(ETeam Team) const
{
	return Team == ETeam::Enemy ? EnemyUnits.Num() : PlayerUnits.Num();
}


void ALaneTSGameMode::RemoveUnitFromTeam(AUnit* Unit)
{
	ETeam UnitsTeam = Unit->GetTeam();
	if(UnitsTeam == ETeam::Enemy)
	{
		if(EnemyUnits.Contains(Unit))
		{
			//double buffering unit list to avoid potential issues of list being accessed/iterated during changes
			PendingEnemyUnitRemovals.Add(Unit);
			//EnemyUnits.Remove(Unit);
		}
		
	}
	else if(UnitsTeam == ETeam::Player)
	{
		if(PlayerUnits.Contains(Unit))
		{
			PendingPlayerUnitRemovals.Add(Unit);
			if(PersistantPlayerUnits.Contains(Unit->UnitName))
			{
				RemovePersistantUnit(Unit->UnitName);
			}
			//PlayerUnits.Remove(Unit);
		}
		
	}
}

void ALaneTSGameMode::AddUnitToTeam(AUnit* Unit, ETeam Team)
{
	ETeam UnitsTeam = Unit->GetTeam();
	if(UnitsTeam == ETeam::Enemy)
	{
		EnemyUnits.Add(Unit);
	}
	else if(UnitsTeam == ETeam::Player)
	{
		PlayerUnits.Add(Unit);
	}
}

float ALaneTSGameMode::CalcTotalRewardWeighting()
{
	float Total = 0;
	for(auto Reward : CachedRewardData)
	{
		Total += Reward.RewardCommanality;
	}
	return Total;
}

void ALaneTSGameMode::GetRewards(TArray<URewardInstance*>&  Rewards, int Amount)
{
	

	for(int i = 0; i < Amount ; i++)
	{
		ERewardRarity RewardRarity = ERewardRarity::Common;
		float TotalWeight = CalcTotalRewardWeighting();
		float RandomValue = FMath::RandRange(0.f, TotalWeight);
		
		for(auto Reward : CachedRewardData)
		{
			if( RandomValue < Reward.RewardCommanality)
			{
				URewardInstance* RewardInst = NewObject<URewardInstance>(this);
				RewardInst->Factor = Reward.GetFactorForRarity(RewardRarity);
				RewardInst->Rarity = RewardRarity;
				RewardInst->RewardTarget = Reward.RewardTarget;
				RewardInst->RewardID = Reward.RewardID;
				RewardInst->Type = Reward.Type;
				Rewards.Add(RewardInst);
			
				if(!Reward.DecrementEntries())
				{
					//not sure how i want to remove rewards from the list right now	
				}
			
				break;
			}
			else
			{
				RandomValue = RandomValue - Reward.RewardCommanality;
			}
		}
		
	}
	
	
}



void ALaneTSGameMode::StartSpawningEnemies()
{
	if(bSpawningEnemies){return;}
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("SpawningEnemies"));
	bSpawningEnemies = true;
	int LastSpawnerIndex = 0;
	for(auto Entry : CurrentWaveData.UnitData)
	{
		for(int i = 0; i < Entry.Amount;i++)
		{
			if(LastSpawnerIndex < RegisteredEnemySpawners.Num())
			{
				RegisteredEnemySpawners[LastSpawnerIndex]->SpawnAtBest(Entry.UnitName,false);
			}
			else
			{
				LastSpawnerIndex = 0;
				RegisteredEnemySpawners[LastSpawnerIndex]->SpawnAtBest(Entry.UnitName,false);
			}
			LastSpawnerIndex++;
		}
	}
	bSpawningEnemies = false;
}

void ALaneTSGameMode::SpawnPlayersUnits_Implementation()
{
	if(bSpawningPlayerUnits){return;}
	
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("SpawningPlayerUnits"));
	int LastSpawnerIndex = 0;
	bSpawningPlayerUnits = true;

	TArray<FName> ProdKeys;
	PlayerUnitProduction.GetKeys(ProdKeys);

	
	for(auto ProdUnit : ProdKeys)
	{
		int Num = GetUnitProduction(ProdUnit);
		for(int i = 0; i < Num; i++)
		{
			if(LastSpawnerIndex < RegisteredPlayerSpawners.Num())
			{
				RegisteredPlayerSpawners[LastSpawnerIndex]->SpawnAtBest(ProdUnit,false);
			}
			else
			{
				LastSpawnerIndex = 0;
				RegisteredPlayerSpawners[LastSpawnerIndex]->SpawnAtBest(ProdUnit,false);
			}
			LastSpawnerIndex++;
		}
	}


	
	TArray<FName> PersKeys;
	PersistantPlayerUnits.GetKeys(PersKeys);
	
	for(auto PersUnit : PersKeys)
	{
		int Num = PersistantPlayerUnits.FindChecked(PersUnit);
		for(int i = 0; i < Num; i++)
		{
			if(LastSpawnerIndex < RegisteredPlayerSpawners.Num())
			{
				RegisteredPlayerSpawners[LastSpawnerIndex]->SpawnAtBest(PersUnit,true);
			}
			else
			{
				LastSpawnerIndex = 0;
				RegisteredPlayerSpawners[LastSpawnerIndex]->SpawnAtBest(PersUnit,true);
			}
			LastSpawnerIndex++;
		}
	}

	
	bSpawningPlayerUnits = false;
}

void ALaneTSGameMode::AddPersistantUnit(FName UnitName, int Amount)
{
	int CurrentAmount = GetUnitProduction(UnitName);
	PlayerUnitProduction.Add(UnitName,CurrentAmount + Amount);
	
}

void ALaneTSGameMode::RemovePersistantUnit(FName UnitName, int Amount)
{
	int CurrentAmount = GetNumPersistantUnits(UnitName);
	if(CurrentAmount - Amount <= 0)
	{
		PersistantPlayerUnits.Remove(UnitName);
		return;
	}
	PersistantPlayerUnits.Add(UnitName,CurrentAmount - Amount);
}

int ALaneTSGameMode::GetNumPersistantUnits(FName UnitName)
{
	int* Num = PersistantPlayerUnits.Find(UnitName);
	if(Num)
	{
		return *Num;
	}
	return 0;
}


int ALaneTSGameMode::GetUnitProduction(FName UnitName)
{
	int* Num = PlayerUnitProduction.Find(UnitName);
	if(Num)
	{
		return *Num;
	}
	return 0;
}

void ALaneTSGameMode::AddUnitProduction(FName UnitName, int Amount)
{
	int CurrentAmount = GetUnitProduction(UnitName);
	PlayerUnitProduction.Add(UnitName,CurrentAmount + Amount);
	
}

void ALaneTSGameMode::RemoveUnitProduction(FName UnitName, int Amount)
{
	int CurrentAmount = GetUnitProduction(UnitName);
	if(CurrentAmount - Amount <= 0)
	{
		PlayerUnitProduction.Remove(UnitName);
		return;
	}
	PlayerUnitProduction.Add(UnitName,CurrentAmount - Amount);
}


void ALaneTSGameMode::PreWave_Implementation()
{
	AUnitPlayerController* PC = Cast<AUnitPlayerController>(UGameplayStatics::GetPlayerController(this,0));
	if(PC)
	{
		if(CurrentWave != FirstWave)
		{
			RestartPlayer(PC);
		}
		
		PC->DisplayGameHUD();
		
	}
	
	PrepareEnemySpawns();
	StartWaveCountdown();
	
}

void ALaneTSGameMode::PrepareEnemySpawns()
{
	if(CurrentWave >= CachedWaveData.Num() ){return;}

	CurrentWaveData = CachedWaveData[CurrentWave];
	
}

void ALaneTSGameMode::StartWave_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("StartingWave"));

	bWaveActive = true;
	OnWaveStartedDispatch.Broadcast();
	SpawnPlayersUnits();
	StartSpawningEnemies();
}

void ALaneTSGameMode::WaveComplete_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("WaveComplete"));
	bWaveActive = false;
	OnWaveEndedDispatch.Broadcast();

	if(CurrentWave == FinalWave)
	{
		GoToWinScreen();
		return;
	}
	
	CurrentWave++;
	
	
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle,this,&ALaneTSGameMode::PostWave,3.f);
}

void ALaneTSGameMode::StartInterWavePhase_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Interphase"));
	AUnitPlayerController* PC = Cast<AUnitPlayerController>(UGameplayStatics::GetPlayerController(this,0));


	
	
	if(PC)
	{
		PC->SwapToSpectatorMode();
		PC->DisplayInterWaveHUD();
		
	}
	PresentRewards();
}

void ALaneTSGameMode::PostWave_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("PostWave"));

	
	ClearUnits();
	StartInterWavePhase();
}

bool ALaneTSGameMode::CheckWaveComplete() const
{
	if(EnemyUnits.Num() <= 0 && !bSpawningEnemies)
	{
		return true;
	}
	return false;
}

void ALaneTSGameMode::ClearUnits()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Clearing Units"));
	//PersistantPlayerUnits.Empty();
	EnemyUnits.Empty();
	PlayerUnits.Empty();
	PendingEnemyUnitRemovals.Empty();
	PendingPlayerUnitRemovals.Empty();
	
	for(TActorIterator<AUnit> ActorItr(GetWorld());ActorItr; ++ActorItr)
	{
		
		if(UGameplayStatics::GetPlayerPawn(this,0) != *ActorItr)
		{
			if(ActorItr->GetTeam() == ETeam::Player && ActorItr->bIsPersistant)
			{
				//AddPersistantUnit(ActorItr->UnitName,1);
			}
			ActorItr->Destroy();
		}
	}
}

void ALaneTSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	//dont go below here unless in a wave
	if(!bWaveActive){return;}
	//going through the buffer at end of frame
	for(auto Unit : PendingEnemyUnitRemovals)
	{
		if(Unit)
		{
			EnemyUnits.Remove(Unit);
		}
	}
	for(auto Unit : PendingPlayerUnitRemovals)
	{
		if(Unit)
		{
			PlayerUnits.Remove(Unit);
		}
	}
	if(bWaveActive  && CheckWaveComplete())
	{
		WaveComplete();
	}
}
