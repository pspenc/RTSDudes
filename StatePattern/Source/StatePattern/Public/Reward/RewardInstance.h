// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Reward/RewardTypes.h"
#include "RewardInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STATEPATTERN_API URewardInstance : public UObject
{
	GENERATED_BODY()

public:

	URewardInstance();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ERewardType Type;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ERewardRarity Rarity;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName RewardTarget;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName RewardID;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Factor;


	
	//check to see if the aggro record matches another aggro record by overloading the "==" operator.
	FORCEINLINE bool operator==(const URewardInstance &Other) const
	{
		if(RewardID == Other.RewardID && Rarity == Other.Rarity)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
