// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Engine/DataAsset.h"
#include "UnitDefintion.generated.h"



/**
 * 
 */
UCLASS()
class STATEPATTERN_API UUnitDefintion : public UPrimaryDataAsset
{
	GENERATED_BODY()


	public:


	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName UnitID;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float Health = 50;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float HealthPerLevel = 50;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float Armor = 0;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float ArmorPerLevel = 0;
	

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float AttackDamage = 10;
	//melee is edge to center, ranged is center to center
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float AttackRange = 1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float AttackSpeed = 1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float AttackSpeedPerLevel = 1;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float MoveSpeed = 600;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	float UnitSize = 1;
	

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Resources)
	int GoldCost;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Resources)
	int IncomeGain;


	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Visuals)
	USkeletalMesh* Mesh;

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = AI)
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = AI)
	AAIController* Controller;

	

	
};
