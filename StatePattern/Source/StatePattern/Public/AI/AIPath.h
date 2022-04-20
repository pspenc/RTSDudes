// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatePattern/StatePattern.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "Team/RTSTeamInterface.h"

#include "AIPath.generated.h"

UCLASS(Blueprintable,BlueprintType,HideCategories = (Physics, Collision, Lighting, Rendering, Mobile), meta=(BlueprintSpawnableComponent))
class STATEPATTERN_API AAIPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIPath();


	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	USceneComponent* PivotComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	USplineComponent* PathSpline;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	ETeam Team;

	UFUNCTION(BlueprintPure)
	float GetPercentProgress(FVector ActorLocation) const;
	//this spline will always be normalized, so 0 is start, 1 is end
	UFUNCTION(BlueprintPure)
	FVector GetLocationForProgress(float Progress) const;
	
	bool bDebugMode;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bSnapToGround;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (EditCondition = "bSnapToGround",EditConditionHides))
	float SnapToGroundTraceDistance;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (EditCondition = "bSnapToGround",EditConditionHides))
	float SnapVerticalOffset;


	
	
protected:
	
	UFUNCTION(BlueprintCallable)
	void RefreshSpline();
	
public:	
	
};
