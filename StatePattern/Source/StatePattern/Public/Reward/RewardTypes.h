// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "RewardTypes.generated.h"

UENUM(BlueprintType)
enum class ERewardRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Legendary
	
};
UENUM(BlueprintType)
enum class ERewardType : uint8
{
	UnitProduction,
	PersistantUnit,
	UnitUpgrade,
	PlayerUpgrade,
	PlayerAbility
	
};


USTRUCT(BlueprintType)
struct FRewardDataEntry : public FTableRowBase
{
	GENERATED_BODY()

	FRewardDataEntry(): CommonFactor(1), UncommonFactor(2), RareFactor(4), LegendaryFactor(6)
	{
		
		Type = ERewardType::UnitProduction;
		NumEntries = 1;
		RewardID = "PeasantProduction";
		RewardTarget = "Peasant";
		RewardCommanality = .3;
	}

	bool DecrementEntries()
	{
		if(NumEntries == -1){return true;}
		if(NumEntries == 0)
		{
			return false;
		}
		NumEntries--;
		return true;
	}
	bool HasEnoughEntries()
	{
		if(NumEntries == -1){return true;}
		return NumEntries > 0;
	}
	bool IsValidWave(int Wave)
	{
		if(Wave > MaxWave && MaxWave!= -1)
		{
			return false;
		}
		if(Wave < MinWave && MinWave != -1)
		{
			return false;
		}
		return true;
	}
	
	FORCEINLINE bool operator()(const FRewardDataEntry& A, const FRewardDataEntry& B) const
	{
		return A.RewardCommanality < B.RewardCommanality;
	}
	bool operator <(const FRewardDataEntry& Other) const
	{
		return RewardCommanality < Other.RewardCommanality;
	}
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName RewardID;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	ERewardType Type;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName RewardTarget;

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float RewardCommanality;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int NumEntries;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int MinWave = 1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int MaxWave = -1;

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int CommonFactor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int UncommonFactor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int RareFactor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int LegendaryFactor;

	int GetFactorForRarity(ERewardRarity Rarity) const
	{
		switch(Rarity)
		{
		case ERewardRarity::Common:
			return CommonFactor;
			break;;
		case ERewardRarity::Uncommon:
			return UncommonFactor;
			break;
		case ERewardRarity::Rare:
			return RareFactor;
			break;
		case ERewardRarity::Legendary:
			return LegendaryFactor;
			break;
		}
		return 1;
	}
	
	
};

