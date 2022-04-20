// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/Projectile.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Unit/Unit.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	SetRootComponent(Collision);

	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjMesh");
	ProjMesh->SetupAttachment(Collision);
	ProjectileParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>("ProjectileParticlesComponent");
	ProjectileParticlesComponent->SetupAttachment(Collision);
	ProjectileParticlesComponent->SetTemplate(TrailVFX);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	GravityScale = 1.f;
	ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
	Speed = 1000;
	Damage = 10.f;
	ImpactVFXScale = FVector(1,1,1);
	//dont want these to proc damage
	SetCanBeDamaged(false);
	bShowBallisticSettings = false;
	bShowInterpolationSettings = false;
	bShowStraightSettings = false;
	bShowHomingSettings = false;
}

void AProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this,&AProjectile::OnProjectileOverlapBegin);
	}
	if(ProjectileMovementComponent)
	{
		ProjectileMovementComponent->OnProjectileStop.AddDynamic(this,&AProjectile::OnProjectileStop);
		
		ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
	}
}

void AProjectile::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
	
}

void AProjectile::Explode()
{
	bHasExploded = true;
	TArray<AActor*> ignoreActors;
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	ignoreActors.Init(this, 1);

	FVector Center = GetActorLocation();

	
	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),Center,ExplodeRadius,ObjectTypes,nullptr,ignoreActors,OutActors);
	if(bDebugMode)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(),Center,ExplodeRadius,12,FLinearColor::Red,.1f,1.f);
	}
	

	PlayHitSound(GetActorLocation());
	for(auto Actor : OutActors)
	{
		if(!Actor){continue;}
		AUnit* Enemy = Cast<AUnit>(Actor);
		if(!Enemy){continue;}
		if(!Enemy->IsAlive()){continue;;}
		if(Enemy->GetTeam() != Team)
		{
			FDamageEvent DamageEvent = FDamageEvent();
			AController* InstigatorController = nullptr;
			if(GetInstigator())
			{
				InstigatorController = GetInstigator()->GetController();				
			}
			Actor->TakeDamage(Damage,FDamageEvent(),InstigatorController,this);
			
		}
	}
	
	if(ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this,ImpactVFX,GetActorLocation(),GetActorRotation(),ImpactVFXScale);
	}
	Destroy();
}

void AProjectile::OnProjectileOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(bExplodes && bHasExploded){return;}
	if(OtherActor == GetInstigator() || OtherActor == GetOwner()){return;}
	
	if(OtherActor->CanBeDamaged())
	{
		if(GetTeamResponseTo(OtherActor) == ETeamResponse::Hostile)
		{
			if(bExplodes)
			{
				Explode();
				return;
			}
			FDamageEvent DamageEvent = FDamageEvent();
			AController* InstigatorController = nullptr;
			if(GetInstigator())
			{
				InstigatorController = GetInstigator()->GetController();				
			}
			OtherActor->TakeDamage(Damage,FDamageEvent(),InstigatorController,this);
			PlayHitSound(GetActorLocation());
			if(ImpactVFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this,ImpactVFX,GetActorLocation(),GetActorRotation(),ImpactVFXScale);
			}
			Destroy();
		}
	}
	
}

#if WITH_EDITOR

bool AProjectile::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	return ParentVal;
	
}

void AProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ProjectileMovementComponent->ProjectileGravityScale = GravityScale;

	return;
	bShowBallisticSettings = MovementType == EProjectileMovementType::Ballistic || ((MovementType == EProjectileMovementType::Homing)
													&& HasDelayBeforeHoming && (PreHomingMovementType == EProjectileMovementType::Ballistic) );

	bShowInterpolationSettings = MovementType == EProjectileMovementType::Interpolated || ((MovementType == EProjectileMovementType::Homing)
													&& HasDelayBeforeHoming && (PreHomingMovementType == EProjectileMovementType::Interpolated) );
	bShowStraightSettings = MovementType == EProjectileMovementType::Straight || ((MovementType == EProjectileMovementType::Homing)
													&& HasDelayBeforeHoming && (PreHomingMovementType == EProjectileMovementType::Straight) );

	bShowHomingSettings = MovementType == EProjectileMovementType::Homing;
	
}
#endif

void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	if(bExplodes && bHasExploded)
	{
		Destroy();
		return;
	}
	else if(bExplodes)
	{
		Explode();
		return;
	}
	if(ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this,ImpactVFX,ImpactResult.Location,GetActorRotation(),ImpactVFXScale);
	}
	PlayHitSound(ImpactResult.Location);
	Destroy();
}

void AProjectile::PlayHitSound(FVector Location)
{
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,HitSound,Location,GetActorRotation());
	}
	
}

void AProjectile::PlayOnSpawnSound()
{
}

void AProjectile::PlaySpawnVFX()
{
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	PlayOnSpawnSound();
	PlaySpawnVFX();
}

