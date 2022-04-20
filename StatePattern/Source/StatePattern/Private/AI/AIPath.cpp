// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIPath.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AAIPath::AAIPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(PivotComponent);
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	PathSpline->SetupAttachment(PivotComponent);
	PathSpline->Duration = 1.0f;
	PathSpline->bDrawDebug = bDebugMode;
	bSnapToGround = true;
	SnapToGroundTraceDistance = 150.f;
	SnapVerticalOffset = 15.f;
	
}

float AAIPath::GetPercentProgress(FVector ActorLocation) const
{
	float PercentProgress = 0.f;
	if(PathSpline)
	{
		const float BestKeyForLocation = PathSpline->FindInputKeyClosestToWorldLocation(ActorLocation);
		const float DistanceAlongSpline = PathSpline->GetDistanceAlongSplineAtSplineInputKey(BestKeyForLocation);
		PercentProgress = FMath::Clamp(DistanceAlongSpline/PathSpline->GetSplineLength(),0.f,1.f);
	}
	return PercentProgress;
}

FVector AAIPath::GetLocationForProgress(float Progress) const
{
	if(PathSpline)
	{
		return PathSpline->GetLocationAtTime(Progress,ESplineCoordinateSpace::World);
	}
	return FVector::ZeroVector;
}


void AAIPath::RefreshSpline()
{
	for(int i = 0; i < PathSpline->GetNumberOfSplinePoints(); i++)
	{
		ETraceTypeQuery TraceTypes = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
		EDrawDebugTrace::Type DebugTraceType = false ? EDrawDebugTrace::ForDuration: EDrawDebugTrace::None;
			
		FHitResult HitResult;
		TArray<AActor*> ignoreActors;
		FVector SplineLocation = PathSpline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::World);
		FVector SplineUpVector = PathSpline->GetUpVectorAtSplinePoint(i,ESplineCoordinateSpace::World);
		
		if(UKismetSystemLibrary::LineTraceSingle(this,SplineLocation,SplineLocation + (SplineUpVector*-SnapToGroundTraceDistance),TraceTypes,false,ignoreActors,
												DebugTraceType,HitResult,true))
		{
			PathSpline->SetLocationAtSplinePoint(i,HitResult.Location+HitResult.Normal*SnapVerticalOffset,ESplineCoordinateSpace::World,true);
			PathSpline->SetUpVectorAtSplinePoint(i,HitResult.Normal,ESplineCoordinateSpace::World,true);
		}
		
	}
}

