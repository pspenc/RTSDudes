// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Unit.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"
#include "Ability/AbilityComponent.h"
#include "Ability/MeleeBasicAttack.h"
#include "Ability/ProjectileAbility.h"
#include "Projectile/Projectile.h"
#include "Components/CapsuleComponent.h"
#include "StatePattern/StatePattern.h"
#include "Kismet/KismetMathLibrary.h"
#include "Unit/UnitAnimInstance.h"
#include "AI/AIPath.h"
#include "AI/UnitAIController.h"
#include "Unit/StateManager.h"

// Sets default values
AUnit::AUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackType = EAttackType::Melee;
	MaxHealth = 100;
	Health = MaxHealth;
	AttackDamage = 10;
	AttackDamagePerLevel = 1.0f;
	MoveSpeed = 600;
	
	UnitSize = 1;

	
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>("AbilityComponent");
	StateManager = CreateDefaultSubobject<UStateManager>("StateManager");
	
	SetCanBeDamaged(true);
}
#if WITH_EDITOR

void AUnit::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ConfigureStatsFromDataTable();
	SetColorsToTeam();
}


#endif


void AUnit::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
	
}

FString AUnit::GetPrettyName()
{
	FString UnrealName = GetNameSafe(this);
	return UnrealName.Append(UnitName.ToString());
}


bool AUnit::CanUseAbilities()
{
	return true;
}

void AUnit::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
	SetColorsToTeam();
}

ETeamResponse AUnit::GetTeamResponseTo(const AActor* Other) const
{
	return IRTSTeamInterface::GetTeamResponseTo(Other);
}


bool AUnit::ConfigureStatsFromDataTable()
{
	UE_LOG(LogDataSystem,Display,TEXT("Configuring Stats for %s : %s"),*GetName(),*UnitName.ToString());
	if(UnitDataTable)
	{
		FUnitData* DataPtr =  UnitDataTable->FindRow<FUnitData>(UnitName,"");
		if(DataPtr)
		{
			Health = DataPtr->Health;
			MaxHealth = DataPtr->Health;
			HealthPerLevel = DataPtr->HealthPerLevel;
			RangedAttackProjectile = DataPtr->ProjectileClass;
			Armor = DataPtr->Armor;
			ArmorPerLevel = DataPtr->ArmorPerLevel;

			AttackRange = DataPtr->AttackRange;
			AttackType = DataPtr->AttackType;
			AttackDamage = DataPtr->AttackDamage;
			AttackDamagePerLevel = DataPtr->AttackDamagePerLevel;
			BehaviorTree = DataPtr->BehaviorTree;
			MeleeHitboxDimensions = DataPtr->MeleeHitboxDimensions;
			AttackSpeed = DataPtr->AttackSpeed;
			AttackSpeedPerLevel = DataPtr->AttackSpeedPerLevel;
			AttackCleaves = DataPtr->AttackCleaves;
			
			MoveSpeed = DataPtr->MoveSpeed;
			MoveSpeedPerLevel = DataPtr->MoveSpeedPerLevel;

			UnitSize = DataPtr->UnitSize;
			GoldCost = DataPtr->GoldCost;
			
			//Animations
			IdleAnim = DataPtr->IdleAnimation;
			MovementBlendSpace = DataPtr->MovementBlendspace;
			AttackMontages = DataPtr->AttackAnimationMontage;
			HitMontages = DataPtr->HitAnimationMontage;
			DeathMontages = DataPtr->DeathMontages;

			
			
			if(GetMesh())
			{
				GetMesh()->SetSkeletalMesh(DataPtr->Mesh);
				
			}
			if(UnitSize > 0)
			{
				SetActorScale3D(FVector(UnitSize,UnitSize,UnitSize));
			}


			
			//grant basic attacks
			if(AttackType == EAttackType::Melee)
			{
				if(!MeleeAttackAbility)
				{
					MeleeAttackAbility = UMeleeBasicAttack::StaticClass();
				}
				BasicAttackAbility = AbilityComponent->GrantAbility(MeleeAttackAbility,1);
				BasicAttackAbility->AbilityName = "BasicAttack";
				BasicAttackAbility->Cooldown = AttackSpeed;
			}
			else if(AttackType == EAttackType::Ranged)
			{
				if(!RangedAttackAbility)
				{
					RangedAttackAbility = UProjectileAbility::StaticClass();
				}
				BasicAttackAbility = AbilityComponent->GrantAbility(RangedAttackAbility,1);
				BasicAttackAbility->AbilityName = "BasicAttack";
				BasicAttackAbility->Cooldown = AttackSpeed;
			}
			
			return true;
		}
	}
	UE_LOG(LogDataSystem,Display,TEXT("%s : Invalid Unit Name for data table : %s"),*GetName(),*UnitName.ToString());
	return false;
}

bool AUnit::HasMoveResource()
{
	if(StateManager->GetMovementResourceHolder() == this)
	{
		return true;
	}
	return false;
}

bool AUnit::HasActionResource()
{
	if(StateManager->GetActionResourceHolder() == this)
	{
		return true;
	}
	return false;
}

bool AUnit::HasAnimationResource()
{
	if(StateManager->GetAnimationResourceHolder() == this)
	{
		return true;
	}
	return false;
}

bool AUnit::TryClaimMoveResource()
{
	if(StateManager)
	{
		return StateManager->ClaimMovementResource(this);
	}
	
	return false;
}

bool AUnit::TryClaimActionResource()
{
	if(StateManager)
	{
		return StateManager->ClaimAnimationResource(this);
	}
	
	return false;
}

bool AUnit::TryClaimAnimationResource()
{
	if(StateManager)
	{
		return StateManager->ClaimAnimationResource(this);
	}
	
	return false;
}

void AUnit::Jump()
{
	if(bIsDead){return;}
	if(StateManager && StateManager->MovementAllowed() && StateManager->ActionsAllowed())
	{
		Super::Jump();
	}
}


AActor* AUnit::GetTarget()
{
	if(AttackTarget)
	{
		return AttackTarget;
	}
	return nullptr;
	
}

bool AUnit::CanBasicAttack()
{
	if(BasicAttackAbility && BasicAttackAbility->CanActivateAbility())
	{
		return true;
	}
	return false;
}

UAbility* AUnit::GetBasicAttackAbility()
{
	return BasicAttackAbility;
}

bool AUnit::Attack(AActor* Target, bool bIgnoreRange)
{
	if(bIsDead){return false;}
	//VLOG_UNIT(this,Display,TEXT("%s attacking %s"),*GetNameSafe(this),*GetNameSafe(Target));
	if(BasicAttackAbility->CanActivateAbility())
	{
		AttackTarget = Target;
	
		return AbilityComponent->ActivateAbility(BasicAttackAbility,Target);
	}
	return false;

}


bool AUnit::IsWithinRange(AActor* Target)
{
	if(bIsDead){return false;}
	if(!Target)
	{
		LOG_UNIT(Display,TEXT("%s cannot check range, target is invalid"),*GetName());
		return false;
	}
	const FVector Start = GetActorLocation();
	FVector End = Target->GetActorLocation();
	const float Distance = UKismetMathLibrary::Vector_Distance(Start,End);
	const float UnitScaledDistance = Distance / GetUnitLength(true);
	
	LOG_UNIT(Display,TEXT("%s range check:%s , Distance : %f, ScaledDistance: %f, UnitAttackRange = %f "),
						*GetName(),*UnitName.ToString(),Distance,UnitScaledDistance,AttackRange);

	return (AttackRange+.25) >= UnitScaledDistance;

}

float AUnit::GetScaledAttackRange()
{
	return (AttackRange) * GetUnitLength(true);
}







UAnimMontage* AUnit::PlayAttackMontage()
{
	if(AttackMontages.Num() > 0)
	{
		const int RandIndex = FMath::RandRange(0,AttackMontages.Num()-1);
		PlayAnimMontage(AttackMontages[RandIndex]);
		return AttackMontages[RandIndex];
	}
	return nullptr;
}

UAnimMontage* AUnit::PlayDeathMontage()
{
	if(DeathMontages.Num() > 0)
	{
		const int RandIndex = FMath::RandRange(0,DeathMontages.Num()-1);
		PlayAnimMontage(DeathMontages[RandIndex]);
		return DeathMontages[RandIndex];
	}
	return nullptr;
}

UAnimMontage* AUnit::PlayHitMontage()
{
	if(HitMontages.Num() <= 0){return nullptr;}

	//Hit Montage is not going to claim anything, since it has much lower priotity.
	if(StateManager->AnimationResourceAvailable())
	{
		const int RandIndex = FMath::RandRange(0,HitMontages.Num()-1);
		PlayAnimMontage(HitMontages[RandIndex]);
		return HitMontages[RandIndex];
	}
	//if we cant play it, dont both. its k.
	return nullptr;
}

void AUnit::FaceActor(AActor* ActorToFace)
{
	if(!ActorToFace){return;}
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),ActorToFace->GetActorLocation());
	NewRotation.Pitch = 0;
	NewRotation.Roll = 0;
	SetActorRotation(NewRotation);
}
FHitbox AUnit::GetDefaultMeleeHitbox()
{
	
	FVector BoxSize;
	BoxSize.X = 10.f;
	BoxSize.Y = GetUnitLength(true)/2.f;
	BoxSize.Z = GetUnitHeight(true)/2.f;

	FVector TraceStart = GetActorTransform().TransformPosition(FVector(UnitSize * (UnitLength/2.f),0,0));
	FVector TraceEnd =  GetActorTransform().TransformPosition(FVector(GetScaledAttackRange(),0,0));

	return FHitbox(BoxSize,TraceStart,TraceEnd);
}

void AUnit::RangedFire()
{
	UE_LOG(LogUnitSystem,Display,TEXT(""),*GetName(),*UnitName.ToString());


	FVector LaunchVelocity;
	
	FVector LaunchLocation = GetActorLocation() + (GetActorForwardVector() * (UnitSize*UnitLength/2.f))
												+ (GetActorUpVector()*(UnitSize*UnitLength/4.f));

	FVector TargetLocation;
	
	if(AttackTarget)
	{
		TargetLocation = AttackTarget->GetActorLocation();
		

		
		bool bUseHighArc = FVector::Distance(LaunchLocation,TargetLocation) > (AttackRange*UnitLength*UnitSize/3.f);

		
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(RangedAttackProjectile,FTransform(LaunchLocation),this,this);
		
		//UGameplayStatics::SuggestProjectileVelocity(this,
		//		LaunchVelocity,LaunchLocation,TargetLocation,
		//		Projectile->Speed,false,0,Projectile->GravityScale*GetWorld()->GetGravityZ(),
		//		ESuggestProjVelocityTraceOption::DoNotTrace,
		//		FCollisionResponseParams::DefaultResponseParam,
		//		TArray<AActor*>(),bDebugMode);	
		
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(this,LaunchVelocity,LaunchLocation,TargetLocation,Projectile->GravityScale*GetWorld()->GetGravityZ(),.7f);
		
		Projectile->Damage = GetAttackDamage();
		Projectile->ProjectileMovementComponent->Velocity = LaunchVelocity;
		Projectile->Team = Team;
		UGameplayStatics::FinishSpawningActor(Projectile,FTransform(LaunchLocation));
		
	}
	
}

FHitResult AUnit::GetTargettingTrace()
{
	FVector TraceStart;
	FRotator Rot;
	GetActorEyesViewPoint(TraceStart,Rot);
	FVector TraceEnd = TraceStart + UKismetMathLibrary::GetForwardVector(Rot) * 4000.f;

	FHitResult HitResult;
	TEnumAsByte<ETraceTypeQuery> TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
	TArray<AActor*> ignoreActors;
	ignoreActors.Add(this);
	EDrawDebugTrace::Type DebugTraceType = bDebugMode ? EDrawDebugTrace::ForDuration: EDrawDebugTrace::None;

	UKismetSystemLibrary::LineTraceSingle(GetWorld(),TraceStart,TraceEnd,TraceType,false,ignoreActors,DebugTraceType,HitResult,
									true,FLinearColor::Green,FLinearColor::Red,.5f);
	return HitResult;
}


void AUnit::Die()
{
	if(bIsDead){return;}
	bIsDead = true;
	OnDied.Broadcast();
	OnDied.Clear();
	SetCanBeDamaged(false);
	
	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>(GetWorld()->GetAuthGameMode());
	if(GM)
	{
		GM->RemoveUnitFromTeam(this);
	}
	
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,this,&AUnit::FinishDying,.6f);
	PlayDeathMontage();
}

void AUnit::FinishDying()
{
	Destroy();
}


float AUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                        AActor* DamageCauser)
{
	if(bIsDead){return 0;}
	float TotalDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
	if(!CanBeDamaged()) return 0;
	float NewHealthValue = FMath::Clamp(Health-TotalDamage,0.f,MaxHealth);
	SetHealth(NewHealthValue);
	
	if(NewHealthValue <= 0) //go to dead state
	{
		Die();
	}
	else    //let current state know
	{
		if(!bIsDead )
		{
			PlayHitMontage();
		}
		
	}
	return TotalDamage;
}

bool AUnit::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser) const
{
	//if parent fails, we fail
	if(!Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser)){return false;}
	if(DamageCauser)
	{
		if(GetTeamResponseTo(DamageCauser) == ETeamResponse::Friendly)
		{
			return false;
		}
	}
	return true;
}

void AUnit::SetAIPath(AAIPath* i_Path)
{
	Path = i_Path;
}

AAIPath* AUnit::GetUnitAIPath()
{
	return Path;
}

float AUnit::GetDistanceForRange(float Range, bool bWithSizeScaling) const
{
	return GetUnitLength(bWithSizeScaling) * Range;
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();

	if(StateManager)
	{
		StateManager->AddActiveState(this);
	}
	
	ConfigureStatsFromDataTable();
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	SetColorsToTeam();
	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>(GetWorld()->GetAuthGameMode());
	if(GM)
	{
		GM->AddUnitToTeam(this,Team);
	}
	
}

void AUnit::SetColorsToTeam()
{
	if(GetMesh())
	{
		GetMesh()->SetMaterial(0,Team == ETeam::Enemy ? EnemyTeamMat:PlayerTeamMat);
	}
}


void AUnit::SetHealth(float NewHealth)
{
	if(NewHealth != Health)
	{
		Health = NewHealth;
		OnHealthChange.Broadcast(NewHealth);
	}
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bDebugAttackRange)
	{
		UKismetSystemLibrary::DrawDebugCircle(this,GetActorLocation(),GetScaledAttackRange(),24,FLinearColor::Red,0,3,
													FVector(1,0,0),FVector(0,1,0),true);
	}
}

// Called to bind functionality to input
void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

