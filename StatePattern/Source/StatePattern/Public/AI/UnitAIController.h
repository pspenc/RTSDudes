// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Unit/Unit.h"
#include "UnitAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPawnAttackFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTargetChanged);
/**
 * 
 */
UCLASS()
class STATEPATTERN_API AUnitAIController : public AAIController
{
	GENERATED_BODY()


	public:

    AUnitAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void StartupBehaviorTree();
	UPROPERTY(BlueprintAssignable)
	FTargetChanged OnTargetChanged;
	UPROPERTY(BlueprintAssignable)
	FPawnAttackFinished OnPawnAttackFinished;
	UFUNCTION()
	void PawnAttackFinishedCallbacks();
	
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OwnedUnitDied();
	UFUNCTION(BlueprintCallable)
	void FocusOnEnemyTarget();
	UFUNCTION(BlueprintCallable)
	void StopFocusOnEnemyTarget();

	UFUNCTION(BlueprintCallable)
	bool AttackTarget(bool IgnoreRange = false);


	UFUNCTION(BlueprintCallable)
	bool SetTarget(AActor* InTarget);

	UFUNCTION(BlueprintPure)
	bool ShouldChangeTarget();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bDebugMode;

	UFUNCTION(BlueprintPure)
	AActor* FindClosestTarget();


	UFUNCTION(BlueprintPure)
	bool HasEnemyTarget() const {return IsValid(EnemyTarget);}
	UFUNCTION(BlueprintPure)
	AActor* GetEnemyTarget() const {return EnemyTarget;}
	UFUNCTION()
	void ClearTarget();
	


	
	UFUNCTION(BlueprintPure)
	class AUnit* GetOwnedUnit() const;

	UFUNCTION(BlueprintPure)
	bool IsFocusingOnActor() const {return bFocusingOnActor;}
	protected:

	bool bFocusingOnActor;
	UPROPERTY()
	AActor* ActorBeingFocused;
	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite)
	AActor* EnemyTarget;

	UPROPERTY(BlueprintReadWrite)
	AUnit* OwnedUnit;

};

