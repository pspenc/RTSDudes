// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UnitStateInterface.h"
#include "AdvObject/AdvancedObject.h"
#include "Engine/DataTable.h"
#include "StatePattern/GameTypes.h"
#include "Ability.generated.h"

UENUM(BlueprintType)
enum class EAbilityTeamTargettingType : uint8
{
	Enemy,
	Friendly,
	Both
};


USTRUCT(BlueprintType)
struct FAbilityDataToImpMapping: public FTableRowBase
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName AbilityID;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<class UAbility> ImplementationClass;
	
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityDelegate,class UAbility*, Ability);

/**
 * 
 */
UCLASS(Abstract,BlueprintType,Blueprintable)
class STATEPATTERN_API UAbility : public UAdvancedObject, public IUnitStateInterface
{
	GENERATED_BODY()


	public:

	UAbility();

	virtual void NativeBeginPlay() override;

	UFUNCTION(BlueprintPure)
	bool IsActive() const {return bIsActive;}

	//checks for related state information, cooldown, and resource costs
	//by default, checks for action resource available, and actions allowed, cooldown and if already active
	UFUNCTION(BlueprintPure)
	virtual bool CanActivateAbility();

	UFUNCTION(BlueprintCallable)
	virtual bool ActivateAbility();

	//finalizes running of ability. Claiming any relavent resources. Last chance to fail
	UFUNCTION(BlueprintCallable)
	virtual bool CommitAbility();

	
	virtual void StartAbility();

	UFUNCTION(BlueprintCallable)
	virtual void EndAbility();

	UPROPERTY(BlueprintAssignable)
	FAbilityDelegate AbilityActivatedCallbacks;
	UPROPERTY(BlueprintAssignable)
	FAbilityDelegate AbilityEndedCallbacks;
	UPROPERTY(BlueprintAssignable)
	FAbilityDelegate AbilityCooldownComplete;

	//cooldown
	UFUNCTION(BlueprintCallable)
	virtual void StartCooldown();
	UFUNCTION(BlueprintCallable)
	virtual void EndCooldown();
	UFUNCTION(BlueprintPure)
	virtual bool IsOnCooldown();
	UFUNCTION(BlueprintPure)
	virtual float GetCooldownLength();
//

	
	UPROPERTY(BlueprintReadOnly)
	int Level;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float Cooldown;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName AbilityName;

	UFUNCTION(BlueprintPure)
	class AUnit* GetOwningUnit() const {return OwningUnit;}
	UFUNCTION(BlueprintCallable)
	void SetOwningUnit(class AUnit* NewOwner);


	void SetAbilityTarget(AActor* Target){AbilityTarget = Target;}
	AActor* GetAbilityTarget() const {return AbilityTarget;}
	bool HasAbilityTarget() const {return IsValid(AbilityTarget);}
	
	protected:

	UPROPERTY()
	class AUnit* OwningUnit;
	UPROPERTY()
	class UStateManager* StateManager;
	
	UPROPERTY()
	AActor* AbilityTarget;

	UFUNCTION(BlueprintCallable)
	class AProjectile* FireProjectile(TSubclassOf<AProjectile> ProjectileClass, FVector Location, FRotator Rotation, AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetTargetsInSphere(FVector Center, float Range, EAbilityTeamTargettingType TeamTargettingType) const;

	UFUNCTION(BlueprintCallable)
	TArray<FHitResult> GetTargetsInBox(FVector Location, FVector Box,FRotator Rotation, EAbilityTeamTargettingType TeamTargettingType) const;
	UFUNCTION(BlueprintCallable)
	TArray<FHitResult> GetTargetsInHitbox(struct FHitbox Hitbox,FRotator Rotation,EAbilityTeamTargettingType TeamTargettingType);
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<ETraceTypeQuery>> AbilityTraceTypes;


	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bDebugMode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bDebugTargettingTraces;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FLinearColor TargettingColor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FLinearColor TargettingHitColor;

	bool bIsActive;
	bool bIsOnCooldown;
	
	UPROPERTY()
	FTimerHandle AbilityCooldownTimer;
	
	

	//Unit State Interface


	virtual bool HasMoveResource() override;
	virtual bool HasActionResource() override;
	virtual bool HasAnimationResource()override;

	virtual bool IsWaitingOnMoveResource()override;
	virtual bool IsWaitingOnActionResource()override;
	virtual bool IsWaitingOnAnimationResource()override;
	
	
	virtual bool TryClaimMoveResource()override;
	virtual bool TryClaimActionResource()override;
	virtual bool TryClaimAnimationResource()override;
	//end Unit state interface

	//extra functions to go alongside the state interface
	virtual void OnPostWaitForMoveResource();
	virtual void OnPostWaitForActionResource();
	virtual void OnPostWaitForAnimationResource();
	
	//defining these since that way, its easy to reset all these, instead of relying on every subclass doing so.
	bool bWaitingForAnimationResource;
	bool bWaitingForMovementResource;
	bool bWaitingForActionResource;

	//end of state related stuff
	
};
