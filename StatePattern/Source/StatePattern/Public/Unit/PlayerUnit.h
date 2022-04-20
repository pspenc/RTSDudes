// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Unit/Unit.h"
#include "PlayerUnit.generated.h"

/**
 * 
 */
UCLASS()
class STATEPATTERN_API APlayerUnit : public AUnit
{
	GENERATED_BODY()


	public:

	APlayerUnit();
	
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	UCameraComponent* Camera;

	//virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	
	protected:
	virtual void FinishDying() override;
	
};
