// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/UnitAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Unit/Unit.h"


AUnitAIController::AUnitAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	bDebugMode = false;

}

void AUnitAIController::StartupBehaviorTree()
{
	if(OwnedUnit)
    {
    	
    	OwnedUnit->OnDied.AddDynamic(this,&AUnitAIController::OwnedUnitDied);
    	OwnedUnit->OnAttackFinished.AddDynamic(this,&AUnitAIController::PawnAttackFinishedCallbacks);
    
    	if(OwnedUnit->BehaviorTree)
    	{
    		RunBehaviorTree(OwnedUnit->BehaviorTree);
    	}
    }
}


void AUnitAIController::PawnAttackFinishedCallbacks()
{
	if(OnPawnAttackFinished.IsBound())
	{
		OnPawnAttackFinished.Broadcast();
		OnPawnAttackFinished.Clear();
	}
}

void AUnitAIController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
}

void AUnitAIController::OnUnPossess()
{
	
	StopFocusOnEnemyTarget();
	
}

void AUnitAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	OwnedUnit = Cast<AUnit>(InPawn);
	FTimerHandle BehaviorDelayHandle;
	GetWorldTimerManager().SetTimer(BehaviorDelayHandle,this,&AUnitAIController::StartupBehaviorTree,1);
	
}

void AUnitAIController::OwnedUnitDied()
{
	GetBrainComponent()->StopLogic("");
}

void AUnitAIController::FocusOnEnemyTarget()
{
	if(!EnemyTarget){return;}
	if(ActorBeingFocused && (ActorBeingFocused != EnemyTarget))
	{
		StopFocusOnEnemyTarget();
	}
	if(EnemyTarget)
	{
		UCharacterMovementComponent* PawnsMovement = GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
		if(PawnsMovement)
		{
			PawnsMovement->bOrientRotationToMovement = false;
		}
		ActorBeingFocused = EnemyTarget;
	
		bFocusingOnActor = true;
		SetActorTickEnabled(true);
	}
	
}

void AUnitAIController::StopFocusOnEnemyTarget()
{
	SetActorTickEnabled(false);
	bFocusingOnActor = false;
	ActorBeingFocused = nullptr;
	UCharacterMovementComponent* PawnsMovement = GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
	if(PawnsMovement)
	{
		PawnsMovement->bOrientRotationToMovement = true;
	}
}


bool AUnitAIController::AttackTarget(bool IgnoreRange)
{
	
	if(!OwnedUnit){return false;}
	if(!EnemyTarget){return false;}

	
	return OwnedUnit->Attack(EnemyTarget,IgnoreRange);
	
}



bool AUnitAIController::SetTarget(AActor* InTarget)
{
	if(InTarget)
	{
		AUnit* CastedTarget = Cast<AUnit>(InTarget);
		
		//make sure target is an enemy
		if(CastedTarget && CastedTarget->GetTeam() != OwnedUnit->GetTeam())
		{
			EnemyTarget = CastedTarget;
			if(!CastedTarget->OnDied.IsAlreadyBound(this,&AUnitAIController::ClearTarget))
			{
				CastedTarget->OnDied.AddDynamic(this,&AUnitAIController::ClearTarget);
			}
		
			OnTargetChanged.Broadcast();
			
			return true;
		}
	}
	return false;
}

bool AUnitAIController::ShouldChangeTarget()
{
	
	if(!EnemyTarget)
	{
		return true; //we have no target, so we should get one
	}
	if(!OwnedUnit)
	{
		return false; //we have no unit, so dont get new target, shouldnt be here
	}
	float DistToTarget = OwnedUnit->GetDistanceTo(EnemyTarget);
	if(DistToTarget <= OwnedUnit->GetScaledAttackRange()*1.5f)
	{
		return false;
	}
	return true;
}

AActor* AUnitAIController::FindClosestTarget()
{
	if(!OwnedUnit){return nullptr;}
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	TArray<AUnit*> Enemies = GM->GetUnitsOnOpposingteam(OwnedUnit->GetTeam());

	float BestDist = 9999;
	AActor* BestTarget = nullptr;
	for (AUnit* Unit : Enemies)
	{
		if(Unit && Unit->GetTeam() != OwnedUnit->GetTeam())
		{
			float Distance = UKismetMathLibrary::Vector_Distance(OwnedUnit->GetActorLocation(),Unit->GetActorLocation());
			if(Distance < BestDist)
			{
				BestTarget = Unit;
				BestDist = Distance;
			}
		}
		
	}

	if(BestTarget)//we found target
	{
		return BestTarget;
	}

	
	
	//not gonna do other searches for now
	return nullptr;
}

void AUnitAIController::ClearTarget()
{
	if(EnemyTarget)
	{
		AUnit* CastedRef = Cast<AUnit>(EnemyTarget);
		if(CastedRef)
		{
			//if(GetBlackboardComponent())
			//{
			//	GetBlackboardComponent()->SetValueAsObject("Target",nullptr);
			//}
			if(CastedRef == ActorBeingFocused)
			{
				StopFocusOnEnemyTarget();
			}
			EnemyTarget = nullptr;
			OnTargetChanged.Broadcast();
			CastedRef->OnDied.RemoveDynamic(this,&AUnitAIController::ClearTarget);
		}
	}
}

AUnit* AUnitAIController::GetOwnedUnit() const
{
	return OwnedUnit;
}

void AUnitAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!GetPawn()){return;}
	if(bFocusingOnActor)
	{
		UCharacterMovementComponent* PawnsMovement = GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
		if(PawnsMovement)
		{

			FRotator CurrentRotation = GetPawn()->GetActorRotation();
			FRotator DesiredRotation = CurrentRotation;
			FRotator DeltaRot = PawnsMovement->GetDeltaRotation(DeltaSeconds);
			
			
			DesiredRotation.Pitch = 0.f;
			DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
			DesiredRotation.Roll = 0.f;
			// Accumulate a desired new rotation.
			const float AngleTolerance = 1e-3f;

			if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
			{
				// PITCH
				if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
				{
					DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
				}

				// YAW
				if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
				{
					DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
				}

				// ROLL
				if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
				{
					DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
				}

				GetPawn()->SetActorRotation(DesiredRotation);
			}
		}
	}
}
