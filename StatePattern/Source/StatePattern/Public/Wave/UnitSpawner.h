// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Team/RTSTeamInterface.h"

#include "UnitSpawner.generated.h"

USTRUCT(BlueprintType)
struct FSpawnLocation
{
	GENERATED_BODY()

	FSpawnLocation()
	{
		LocalLocation = FVector(0,0,0);
		TimeSinceLastSpawn = 0;
	}

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector LocalLocation;
	UPROPERTY()
	float TimeSinceLastSpawn;

	float IncrementTime(float DeltaTime,float Cap)
	{
		TimeSinceLastSpawn = FMath::Min(TimeSinceLastSpawn+DeltaTime,Cap);
		return TimeSinceLastSpawn;
	}
	
};


UCLASS(Blueprintable,BlueprintType)
class STATEPATTERN_API AUnitSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitSpawner();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<class AUnit> UnitBaseClass;

	UPROPERTY(EditInstanceOnly,BlueprintReadOnly)
	ETeam SpawnerTeam;
	UFUNCTION(BlueprintPure)
	FSpawnLocation GetBestSpawnLocation();

	UFUNCTION(BlueprintCallable)
	void SpawnAtBest(FName UnitName,bool bIsPersistant = false);

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float SpawnerDesiredCooldown;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float LastSpawnTimeCap;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FSpawnLocation> SpawnLocations;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	int CurrentBestIndex;
	//FSpawnLocation CurrentBestSpawnLocation;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
