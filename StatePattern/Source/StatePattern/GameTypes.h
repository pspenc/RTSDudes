// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "GameTypes.generated.h"



USTRUCT(BlueprintType)
struct FHitbox
{
	GENERATED_BODY()

	FHitbox()
	{
		BoxExtents = FVector::ZeroVector;
		Start = FVector::ZeroVector;
		End = FVector::ZeroVector;
	}
	
	FHitbox(FVector Extents, FVector StartLocation, FVector EndLocation)
	{
		BoxExtents = Extents;
		Start = StartLocation;
		End = EndLocation;
	}
	
	UPROPERTY()
	FVector BoxExtents;
	UPROPERTY()
	FVector Start;
	UPROPERTY()
	FVector End;
	
};