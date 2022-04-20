// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitStateInterface.h"
#include "StateManager.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STATEPATTERN_API UStateManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateManager();

	
	
	UFUNCTION(BlueprintPure)
	bool ActionsAllowed();
	UFUNCTION(BlueprintPure)
	bool MovementAllowed();
	

	
	UFUNCTION(BlueprintPure)
	bool AnimationResourceAvailable();
	UFUNCTION(BlueprintPure)
	bool ActionResourceAvailable();
	UFUNCTION(BlueprintPure)
	bool MovementResourceAvailable();

	bool ClaimAnimationResource(TScriptInterface<IUnitStateInterface> ClaimantState);
	bool ClaimActionResource(TScriptInterface<IUnitStateInterface> ClaimantState);
	bool ClaimMovementResource(TScriptInterface<IUnitStateInterface> ClaimantState);

	void ReturnAnimationResource(TScriptInterface<IUnitStateInterface> ReturningState);
	void ReturnActionResource(TScriptInterface<IUnitStateInterface> ReturningState);
	void ReturnMovementResource(TScriptInterface<IUnitStateInterface> ReturningState);
	
	
	void AddActiveState(TScriptInterface<IUnitStateInterface> State);
	void RemoveActiveState(TScriptInterface<IUnitStateInterface> State);



	
	FORCEINLINE TScriptInterface<IUnitStateInterface> GetAnimationResourceHolder(){return AnimationResourceHolder;}
	FORCEINLINE TScriptInterface<IUnitStateInterface> GetActionResourceHolder(){return ActionResourceHolder;}
	FORCEINLINE TScriptInterface<IUnitStateInterface> GetMovementResourceHolder(){return MovementResourceHolder;}
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CheckForWaitingStates();
	void CleanupRemovedStates();
	TArray<TScriptInterface<IUnitStateInterface>> ActiveStates;
	
	TArray<TScriptInterface<IUnitStateInterface>> StatesToRemove;
	TArray<TScriptInterface<IUnitStateInterface>> NextFrameStatesToAdd;
	
	UPROPERTY()
	TScriptInterface<IUnitStateInterface> MovementResourceHolder;
	UPROPERTY()
	TScriptInterface<IUnitStateInterface> ActionResourceHolder;
	UPROPERTY()
	TScriptInterface<IUnitStateInterface> AnimationResourceHolder;
	
	bool bActiveListLocked;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
