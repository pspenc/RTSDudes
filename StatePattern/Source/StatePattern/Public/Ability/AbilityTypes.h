// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "AbilityTypes.generated.h"



USTRUCT(BlueprintType)
struct FAbilityContext
{
	GENERATED_BODY()

	AActor* Owner;

		
	
};



USTRUCT(BlueprintType)
struct FAbilityTargetData
{
	GENERATED_BODY()



	
	void AddHitResult(FHitResult NewHit)
	{
		HitResultTargets.Add(NewHit);
	}
	void AddHitResults(TArray<FHitResult> Hits)
	{
		HitResultTargets.Append(Hits);
	}
	void AddActorTarget(AActor* NewActorTarget)
	{
		ActorTargets.Add(NewActorTarget);
	}
	void AddActorTargets(TArray<AActor*> NewActorTargets)
	{
		ActorTargets.Append(NewActorTargets);
	}
	
	UPROPERTY()
	TArray<AActor*> ActorTargets;
 	UPROPERTY()
	TArray<FHitResult> HitResultTargets;
	
};