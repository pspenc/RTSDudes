// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnitStateInterface.generated.h"

class UStateManager;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUnitStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STATEPATTERN_API IUnitStateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	
	virtual bool BlocksMovement();
	virtual bool BlocksActions();


	virtual bool HasMoveResource() = 0;
	virtual bool HasActionResource() = 0;
	virtual bool HasAnimationResource() = 0;

	virtual bool IsWaitingOnMoveResource();
	virtual bool IsWaitingOnActionResource();
	virtual bool IsWaitingOnAnimationResource();
	
	virtual bool TryClaimMoveResource() = 0;
	virtual bool TryClaimActionResource() = 0;
	virtual bool TryClaimAnimationResource() = 0;
};
