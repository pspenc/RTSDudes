// Fill out your copyright notice in the Description page of Project Settings.


#include "Wave/UnitSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Unit/Unit.h"

// Sets default values
AUnitSpawner::AUnitSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LastSpawnTimeCap = 2.f;
	SpawnLocations.Init(FSpawnLocation(),1);
	CurrentBestIndex = 0;
	UnitBaseClass = AUnit::StaticClass();
}

FSpawnLocation AUnitSpawner::GetBestSpawnLocation()
{
	for(int i = 0; i < SpawnLocations.Num(); i++)
	{
		if(SpawnLocations[i].TimeSinceLastSpawn > SpawnLocations[CurrentBestIndex].TimeSinceLastSpawn)
		{
			CurrentBestIndex = i;
		}
	}
	return SpawnLocations[CurrentBestIndex];
}

void AUnitSpawner::SpawnAtBest(FName UnitName,bool bIsPersistant)
{

	FSpawnLocation SpawnLoc = GetBestSpawnLocation();
	FVector WorldLocation = GetActorTransform().TransformPosition(SpawnLoc.LocalLocation);
	
	AUnit* NewUnit = GetWorld()->SpawnActorDeferred<AUnit>(UnitBaseClass,FTransform(GetActorRotation(),WorldLocation));
	NewUnit->UnitName = UnitName;
	NewUnit->SetTeam(SpawnerTeam);
	NewUnit->bIsPersistant = bIsPersistant;
	UGameplayStatics::FinishSpawningActor(NewUnit,FTransform(GetActorRotation(),WorldLocation));
	SpawnLoc.TimeSinceLastSpawn = GetWorld()->DeltaTimeSeconds;
	SpawnLocations[CurrentBestIndex] = SpawnLoc;
}


// Called when the game starts or when spawned
void AUnitSpawner::BeginPlay()
{
	Super::BeginPlay();
	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>(UGameplayStatics::GetGameMode(this));
	if(GM)
	{
		GM->RegisterSpawner(this,SpawnerTeam);
	}
}

// Called every frame
void AUnitSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for(int i = 0; i < SpawnLocations.Num(); i++)
	{
		if(SpawnLocations[i].IncrementTime(DeltaTime,LastSpawnTimeCap) > SpawnLocations[CurrentBestIndex].TimeSinceLastSpawn)
		{
			CurrentBestIndex = i;
		}
	}
	
}

