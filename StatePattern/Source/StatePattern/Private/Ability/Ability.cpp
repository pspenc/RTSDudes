// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/Ability.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "StatePattern/StatePattern.h"
#include "Projectile/Projectile.h"
#include "Unit/StateManager.h"
#include "Unit/Unit.h"

UAbility::UAbility()
{
	Level = 1;
	
	Cooldown = 1.f;
	bIsActive = false;
	bIsOnCooldown = false;		
	TargettingColor = FLinearColor::Red;
	TargettingHitColor = FLinearColor::Green;
	
}

void UAbility::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if(GetOwningActor())
	{
		AUnit* CastedOwner = Cast<AUnit>(GetOwningActor());
		if(CastedOwner)
		{
			OwningUnit = CastedOwner;
			StateManager = OwningUnit->FindComponentByClass<UStateManager>();
			
		}
	}
}

bool UAbility::CanActivateAbility()
{
	return !IsOnCooldown() && StateManager->ActionResourceAvailable() && StateManager->ActionsAllowed() &&!IsActive();
}

bool UAbility::ActivateAbility()
{
	if(CanActivateAbility())
	{
		if(CommitAbility())
		{
			AbilityActivatedCallbacks.Broadcast(this);
			StartAbility();
			return true;
		}
	}
	return false;
}

bool UAbility::CommitAbility()
{
	//if already active, we cannot commit
	if(bIsActive){return false;}
	//double check activation conditions
	if(!CanActivateAbility()){return false;}
	//grab the action resource
	if(!TryClaimActionResource()){return false;}

	//set ourselves as active
	bIsActive = true;
	
	return true;
}

void UAbility::SetOwningUnit(AUnit* NewOwner)
{
	OwningUnit = NewOwner;
	StateManager = OwningUnit->StateManager;
}

void UAbility::StartAbility()
{
	//default behavior, just end next frame.
	//we do next frame to avoid any race condition problems. Plus im just assuming its better practice
	GetWorld()->GetTimerManager().SetTimerForNextTick(this,&UAbility::EndAbility);
}

void UAbility::EndAbility()
{
	//if not active, dont do any other end logic
	if(!bIsActive){return;}
	
	StartCooldown();
	//flip active state
	bIsActive = false;
	bWaitingForActionResource = false;
	bWaitingForAnimationResource = false;
	bWaitingForMovementResource = false;
	if(StateManager)
	{
		/*
		*	any resources we have will get auto cleaned up when we end like this
		*	the State manager will run through its resource holders, and if it matches this,
		*	it will remove it
		*/
		StateManager->RemoveActiveState(this);
	}
	AbilityEndedCallbacks.Broadcast(this);
}

void UAbility::StartCooldown()
{
	if(bIsOnCooldown)
	{
		
	}
	UE_LOG(LogUnitSystem,Display,TEXT("%s : Starting Cooldown of length %d"),*GetName(),Cooldown);
	GetWorld()->GetTimerManager().SetTimer(AbilityCooldownTimer,this,&UAbility::EndCooldown,GetCooldownLength());
	bIsOnCooldown = true;
}

void UAbility::EndCooldown()
{
	if(!bIsOnCooldown)
	{
		
	}
	AbilityCooldownComplete.Broadcast(this);
	AbilityCooldownComplete.Clear();
	UE_LOG(LogUnitSystem,Display,TEXT("%s :  Cooldown completed"),*GetName());
	bIsOnCooldown = false;
	GetWorld()->GetTimerManager().ClearTimer(AbilityCooldownTimer);
}

bool UAbility::IsOnCooldown()
{
	return bIsOnCooldown;
}

float UAbility::GetCooldownLength()
{
	return Cooldown;
}


AProjectile* UAbility::FireProjectile(TSubclassOf<AProjectile> ProjectileClass, FVector Location, FRotator Rotation, AActor* Target)
{
	if(!ProjectileClass){return nullptr;}
	if(!GetOwningActor()){return nullptr;}

	
	const FTransform LaunchTransform = FTransform(Rotation,Location,FVector(1,1,1));

	UE_LOG(LogUnitSystem,Display,TEXT("%s : Firing Projectile of class %s : Loc=%s, Rot=%s "),*GetName(),*ProjectileClass->GetName(),*Location.ToCompactString(),*Rotation.ToCompactString());
	APawn* OwningPawn = Cast<APawn>(GetOwningActor());
	
	if(OwningPawn->IsPlayerControlled() || !Target)
	{
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass,LaunchTransform,GetOwningActor(),OwningPawn);
		Projectile->Team = IRTSTeamInterface::GetActorsTeam(GetOwningActor());
		Projectile->Level = Level;
		Projectile->ProjectileMovementComponent->InitialSpeed = Projectile->Speed;
		UGameplayStatics::FinishSpawningActor(Projectile,LaunchTransform);
	
		return Projectile;
	}
	else
	{
		FVector LaunchVelocity;
		
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass,LaunchTransform,GetOwningActor(),OwningPawn);

		UGameplayStatics::SuggestProjectileVelocity_CustomArc(this,LaunchVelocity,Location,Target->GetActorLocation(),Projectile->GravityScale*GetWorld()->GetGravityZ(),.7f);
		Projectile->Damage = OwningUnit->GetAttackDamage();
		Projectile->ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		Projectile->Team = IRTSTeamInterface::GetActorsTeam(GetOwningActor());
		Projectile->Level = Level;
		Projectile->ProjectileMovementComponent->Velocity = LaunchVelocity;
		UGameplayStatics::FinishSpawningActor(Projectile,LaunchTransform);
		return Projectile;
		
	}
	
}

TArray<AActor*> UAbility::GetTargetsInSphere(FVector Center, float Range, EAbilityTeamTargettingType TeamTargettingType) const
{
	TArray<AActor*> ValidTargets = TArray<AActor*>();
	if(!GetOwningActor()) {return ValidTargets;}
	
	TArray<AActor*> ignoreActors;
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	ignoreActors.Init(GetOwningActor(), 1);


	AUnit* Owner = Cast<AUnit>(GetOwningActor());
	
	
	const float ScaledRadius = Owner->GetDistanceForRange(Range,true);
	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),Center,ScaledRadius,ObjectTypes,nullptr,ignoreActors,OutActors);
	if(bDebugMode && bDebugTargettingTraces)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(),Center,ScaledRadius,12,TargettingColor,.1f,1.f);
	}

	for(auto Actor : OutActors)
	{
		if(TeamTargettingType == EAbilityTeamTargettingType::Both)
		{
			ValidTargets.Add(Actor);
		}
		else
		{
			const ETeamResponse TeamResponse = Owner->GetTeamResponseTo(Actor);
			if(TeamResponse == ETeamResponse::Friendly &&
				TeamTargettingType == EAbilityTeamTargettingType::Friendly)
			{
			
			}
			else if(TeamResponse == ETeamResponse::Hostile &&
				TeamTargettingType == EAbilityTeamTargettingType::Enemy)
			{
				ValidTargets.Add(Actor);
			}
		}
		
	}

	return ValidTargets;
	
}

TArray<FHitResult> UAbility::GetTargetsInBox(FVector Location, FVector Box, FRotator Rotation,
	EAbilityTeamTargettingType TeamTargettingType) const
{
	TArray<AActor*> ValidTargets = TArray<AActor*>();
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	EDrawDebugTrace::Type DebugTraceType = bDebugMode ? EDrawDebugTrace::ForDuration: EDrawDebugTrace::None;
	TArray<FHitResult> HitResults;
	TArray<FHitResult> ValidHitResults;
	TArray<AActor*> ignoreActors;
	AUnit* Owner = Cast<AUnit>(GetOwningActor());
	ignoreActors.Init(GetOwningActor(), 1);

	if(UKismetSystemLibrary::BoxTraceMulti(this,Location,Location+FVector(0,0,1),Box,Rotation,
										UEngineTypes::ConvertToTraceType(COLLISION_DAMAGE),false,ignoreActors,DebugTraceType,
										HitResults,true,FLinearColor::Red,FLinearColor::Green,1.f))
	{
		for(auto Hit  : HitResults)
		{
			if(TeamTargettingType == EAbilityTeamTargettingType::Both)
			{
				ValidHitResults.Add(Hit);				
				
			}
			else
			{
				const ETeamResponse TeamResponse = Owner->GetTeamResponseTo(Hit.GetActor());
				if(TeamResponse == ETeamResponse::Friendly &&
					TeamTargettingType == EAbilityTeamTargettingType::Friendly)
				{
					ValidHitResults.Add(Hit);			
					
				}
				else if(TeamResponse == ETeamResponse::Hostile &&
					TeamTargettingType == EAbilityTeamTargettingType::Enemy)
				{
					ValidHitResults.Add(Hit);			
					
				}
			}
			
			
		}
	}
	return ValidHitResults;
	
}

TArray<FHitResult> UAbility::GetTargetsInHitbox(FHitbox Hitbox, FRotator Rotation,
	EAbilityTeamTargettingType TeamTargettingType)
{
	TArray<FHitResult> HitResults;
	TArray<FHitResult> FilteredHits;
	TArray<AActor*> ignoreActors;
	EDrawDebugTrace::Type DebugTraceType = bDebugMode ? EDrawDebugTrace::ForDuration: EDrawDebugTrace::None;
	
	if(UKismetSystemLibrary::BoxTraceMulti(this,Hitbox.Start,Hitbox.End,Hitbox.BoxExtents,Rotation,
										UEngineTypes::ConvertToTraceType(COLLISION_DAMAGE),false,ignoreActors,DebugTraceType,
										HitResults,true,FLinearColor::Red,FLinearColor::Green,1.f))
	{
		if(TeamTargettingType == EAbilityTeamTargettingType::Both)
		{
			return HitResults;
		}
		for(auto Hit  : HitResults)
		{
			
			
			const ETeamResponse TeamResponse = OwningUnit->GetTeamResponseTo(Hit.GetActor());
			if(TeamResponse == ETeamResponse::Friendly &&
				TeamTargettingType == EAbilityTeamTargettingType::Friendly)
			{
				FilteredHits.Add(Hit);
				
			}
			else if(TeamResponse == ETeamResponse::Hostile &&
				TeamTargettingType == EAbilityTeamTargettingType::Enemy)
			{
				FilteredHits.Add(Hit);
			}
		}
	}
	return FilteredHits;
}

bool UAbility::HasMoveResource()
{
	if(StateManager->GetMovementResourceHolder() == this)
	{
		return true;
	}
	return false;
}

bool UAbility::HasActionResource()
{
	if(StateManager->GetActionResourceHolder() == this)
	{
		return true;
	}
	return false;
}

bool UAbility::HasAnimationResource()
{
	if(StateManager->GetAnimationResourceHolder() == this)
	{
		return true;
	}
	return false;
}

bool UAbility::IsWaitingOnMoveResource()
{
	return IsActive() && bWaitingForMovementResource;
}

bool UAbility::IsWaitingOnActionResource()
{
	return IsActive() && bWaitingForActionResource;
}

bool UAbility::IsWaitingOnAnimationResource()
{
	return IsActive() && bWaitingForAnimationResource;
}

bool UAbility::TryClaimMoveResource()
{
	if(StateManager)
	{
		if(StateManager->ClaimMovementResource(this))
		{	//were able to get resource
			if(bWaitingForMovementResource)
			{
				
				OnPostWaitForMoveResource();
			}
			return true;
		}
	}
	
	return false;
}

bool UAbility::TryClaimActionResource()
{
	if(StateManager)
	{
		if(StateManager->ClaimActionResource(this))
		{	//were able to get resource
			if(bWaitingForActionResource)
			{
				
				OnPostWaitForActionResource();
			}
			return true;
		}
	}
	
	return false;
}

bool UAbility::TryClaimAnimationResource()
{
	if(StateManager)
	{
		if(StateManager->ClaimAnimationResource(this))
		{	//were able to get resource
			if(bWaitingForAnimationResource)
			{
				OnPostWaitForAnimationResource();
			}
			return true;
		}
	}
	
	return false;
}

void UAbility::OnPostWaitForMoveResource()
{
	bWaitingForMovementResource = false;
}

void UAbility::OnPostWaitForActionResource()
{
	bWaitingForActionResource = false;
}

void UAbility::OnPostWaitForAnimationResource()
{
	bWaitingForAnimationResource = false;
}
