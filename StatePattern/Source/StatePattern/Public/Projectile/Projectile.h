// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "StatePattern/StatePattern.h"
#include "Team/RTSTeamInterface.h"

#include "Projectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileMovementType :uint8
{
	Ballistic,
	Homing,
	Interpolated,
	Straight
};

UCLASS()
class STATEPATTERN_API AProjectile : public AActor, public IRTSTeamInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	virtual void PostInitializeComponents() override;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement)
	EProjectileMovementType MovementType;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta=(ExposeOnSpawn="true"))
	ETeam Team;

	//IRTS TEAM INTERFACE

	virtual void SetTeam(ETeam NewTeam) override;
	virtual ETeam GetTeam() const override {return Team;}
	
	//
	bool bGivenExternalVelocity;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta=(ExposeOnSpawn="true"))
	float Damage;
	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn="true"))
	int Level;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool bExplodes;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool bExplodeRadius;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool bHasExploded = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ExplodeRadius = 300.f;
	void Explode();
	bool bDebugMode = false;
	UFUNCTION()
	void OnProjectileOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	bool bShowInterpolationSettings;
	bool bShowBallisticSettings;
	bool bShowHomingSettings;
	bool bShowStraightSettings;
#if WITH_EDITOR

	virtual bool CanEditChange(const FProperty* InProperty) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
	

	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement, meta =(EditCondition = "bShowBallisticSettings||bShowStraightSettings",EditConditionHides))											
	float Speed;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement, meta =(EditCondition = "bShowBallisticSettings",EditConditionHides))
	float GravityScale;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowInterpolationSettings",EditConditionHides))
	UCurveVector* InterpolationCurve;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowInterpolationSettings",EditConditionHides))
	float InterpolationLength;

	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowHomingSettings",EditConditionHides))
	float DelayBeforeHoming;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowHomingSettings",EditConditionHides))
	bool HasDelayBeforeHoming;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowHomingSettings",EditConditionHides))
	EProjectileMovementType PreHomingMovementType;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowHomingSettings",EditConditionHides))
	float HomingSpeed;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowHomingSettings",EditConditionHides))
	float SnapDistance;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Movement,meta =(EditCondition = "bShowHomingSettings",EditConditionHides))
	float SnapSpeed;
	
	
	
	//Components
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* ProjMesh;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UParticleSystemComponent* ProjectileParticlesComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UParticleSystem* TrailVFX;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector ImpactVFXScale;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UParticleSystem* ImpactVFX;
	
	
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult); 



	//end components
	//

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	USoundBase* HitSound;
	UFUNCTION()
	void PlayHitSound(FVector Location);
	UFUNCTION()
	void PlayOnSpawnSound();
	UFUNCTION()
	void PlaySpawnVFX();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
};
