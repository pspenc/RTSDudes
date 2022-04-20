// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "Animation/BlendSpace1D.h"
#include "Data/Unit/Stat/UnitDefintion.h"
#include "Engine/DataTable.h"
#include  "StatePattern/Public/LaneTSGameMode.h"
#include "GameFramework/Character.h"
#include "StatePattern/GameTypes.h"
#include "StatePattern/StatePattern.h"

#include "UnitStateInterface.h"
#include "Team/RTSTeamInterface.h"


#include "Unit.generated.h"



class UUnitState;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee,
	Ranged
};

USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName UnitID;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float Health = 50;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float HealthPerLevel = 10;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float Armor = 0;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float ArmorPerLevel = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	EAttackType AttackType = EAttackType::Melee;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float AttackDamage = 10;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float AttackDamagePerLevel = 1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	bool AttackCleaves = false;
	
	//melee is edge to center, ranged is center to center
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float AttackRange = 1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	FVector MeleeHitboxDimensions = FVector(1,.75,.75);
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	TSubclassOf<class AProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float AttackSpeed = 1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float AttackSpeedPerLevel = 1;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float MoveSpeed = 600;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float MoveSpeedPerLevel = 600;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Stats")
	float UnitSize = 1;
	

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Stats")
	int GoldCost;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Visuals)
	USkeletalMesh* Mesh;

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = AI)
	UBehaviorTree* BehaviorTree;
	

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Animation")
	UAnimSequence* IdleAnimation;
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Animation")
	TArray<UAnimMontage*> HitAnimationMontage;
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Animation")
	TArray<UAnimMontage*> AttackAnimationMontage;
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Animation")
	TArray<UAnimMontage*> DeathMontages;
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Animation")
	UBlendSpace1D* MovementBlendspace;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateStackChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChange, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackStarted,AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);


UCLASS(HideCategories = "Replication,Input")
class STATEPATTERN_API AUnit : public ACharacter, public IRTSTeamInterface, public IUnitStateInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnit();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bDebugAttackRange;

	bool bIsPersistant = false;
	
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostInitializeComponents() override;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class UAbility> MeleeAttackAbility;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class UAbility> RangedAttackAbility;
	

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStateManager* StateManager;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ExposeOnSpawn="true"))	
	FName UnitName;

	UFUNCTION(BlueprintCallable)
	virtual FString GetPrettyName();

	UPROPERTY(BlueprintAssignable)
	FOnDied OnDied;
	UFUNCTION(BlueprintPure)
	bool IsAlive(){return !bIsDead;}

	UFUNCTION(BlueprintPure)
	bool CanUseAbilities();

	//irts team interface

	UFUNCTION(BlueprintPure)
	virtual ETeam GetTeam() const override {return Team;}
	UFUNCTION(BlueprintCallable)
	virtual void SetTeam(ETeam NewTeam) override;
	UFUNCTION(BlueprintPure)
	virtual ETeamResponse GetTeamResponseTo(const AActor* Other) const override;


	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly)
	class UAbilityComponent* AbilityComponent;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<TSubclassOf<class UAbility>> DefaultAbilities;

	
	//Data
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = Data)
	UDataTable* UnitDataTable;

	UFUNCTION(BlueprintCallable)
	bool ConfigureStatsFromDataTable();
	//end data
	
	//Unit State Interface


	virtual bool HasMoveResource() override;
	virtual bool HasActionResource()override;
	virtual bool HasAnimationResource()override;

	
	virtual bool TryClaimMoveResource()override;
	virtual bool TryClaimActionResource()override;
	virtual bool TryClaimAnimationResource()override;
	//end Unit state interface

	
	virtual void Jump() override;
	
	

	//** Animations//
	//these grab a animation from the potential list, and returns the one one, so that bindings and such can be done.
	UFUNCTION(BlueprintCallable)
	UAnimMontage* PlayAttackMontage();
	UFUNCTION(BlueprintCallable)
	UAnimMontage* PlayDeathMontage();
	UFUNCTION(BlueprintCallable)
	UAnimMontage* PlayHitMontage();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBlendSpace1D* MovementBlendSpace;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimSequence* IdleAnim;
	
	
	////


	//gets the units current desired attack target. For AI, thislly use their behavior tree, for players, this can do a trace to find a target
	UFUNCTION(BlueprintPure)
	AActor* GetTarget();

	UFUNCTION(BlueprintPure)
	bool CanBasicAttack();
	UFUNCTION(BlueprintPure)
	UAbility* GetBasicAttackAbility();
	
	UFUNCTION(BlueprintCallable)
	bool Attack(AActor* Target, bool bIgnoreRange = false);
	
	UPROPERTY(BlueprintAssignable)
	FOnAttackStarted OnAttackStarted;
	UPROPERTY(BlueprintAssignable)
	FOnAttackFinished OnAttackFinished;

	UFUNCTION(BlueprintPure)
	bool IsWithinRange(AActor* Target);
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AttackRange = 1;

	UFUNCTION(BlueprintPure)
	float GetScaledAttackRange();

	/*
	* Normalized to unit scale values. So an X of 1, means 1 unit length away. X is also used for the length of trace, not actual trace box, the box X value is
	* hardcoded at 10. However the Y and Z values are still used for the trace box dimensions
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Attack")																									 
	FVector MeleeHitboxDimensions;

	UFUNCTION(BlueprintPure)
	FHitbox GetDefaultMeleeHitbox();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Stats)
	bool AttackCleaves = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess="true"))
	TSubclassOf<class AProjectile> RangedAttackProjectile;
	

	UFUNCTION(BlueprintCallable)
	void FaceActor(AActor* ActorToFace);
	
	UPROPERTY(BlueprintReadOnly)
	AActor* AttackTarget;
	

	UFUNCTION()
	void RangedFire();

	UFUNCTION(BlueprintPure)
	virtual FHitResult GetTargettingTrace();

	/**
	* Apply damage to this actor.
	* @see https://www.unrealengine.com/blog/damage-in-ue4
	* @param DamageAmount		How much damage to apply
	* @param DamageEvent		Data package that fully describes the damage received.
	* @param EventInstigator	The Controller responsible for the damage.
	* @param DamageCauser		The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	* @return					The amount of damage actually applied.
	*/
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

	virtual void Die();
	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;
	UPROPERTY(BlueprintAssignable)
	FOnStateStackChanged OnStateStackChanged;
	

	UFUNCTION(BlueprintCallable)
	void SetAIPath(class AAIPath* i_Path);
	UFUNCTION(BlueprintPure)
	class AAIPath* GetUnitAIPath();
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class AAIPath* Path;
	//AI
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = AI)
	UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintPure)
	float GetUnitLength(bool bWithSizeScaling = true) const {return bWithSizeScaling? UnitLength*UnitSize : UnitLength;}
	
	UFUNCTION(BlueprintPure)
	float GetUnitHeight(bool bWithSizeScaling = true) const {return bWithSizeScaling? UnitHeight*UnitSize : UnitHeight;}
	
	UFUNCTION(BlueprintPure)
	float GetDistanceForRange(float Range,bool bWithSizeScaling) const;


	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	UAbility* BasicAttackAbility;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	float UnitLength = 100.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	float UnitHeight = 80.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Visuals)
	USkeletalMesh* UnitMesh;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	UMaterialInstance* PlayerTeamMat;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	UMaterialInstance* EnemyTeamMat;

	UFUNCTION(BlueprintCallable)
	void SetColorsToTeam();


	
	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,meta=(AllowPrivateAccess="true",ExposeOnSpawn="true"))
	ETeam Team;




	
	
	//Stats
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Defense")
	float MaxHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Defense")
	float Health;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Defense")
	float HealthPerLevel;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Defense")
	float Armor;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Defense")
	float ArmorPerLevel;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Offense")
	float AttackDamage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Offense")
	float AttackDamagePerLevel;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Offense")
	EAttackType AttackType;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Offense")
	float AttackSpeed;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Offense")
	float AttackSpeedPerLevel;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Movement")
	float MoveSpeed;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats|Movement")
	float MoveSpeedPerLevel;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Stats")
	float UnitSize;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Stats|Cost")
	int GoldCost;


	
	
	//end stats
//***
//**	Attack Variables


	
	
	UPROPERTY(Transient,BlueprintReadWrite)
	bool bIsHitReacting;
	

	
	virtual void FinishDying();
	bool bIsDead = false;
	
	//IUnitStateInterface

	bool bQueuedHitMontage;
	float QueudHitMontageTimestamp;
	
	
	//end  IUnitStateInterface


	//animation
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Montage")
	TArray<UAnimMontage*> HitMontages;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Montage")
	TArray<UAnimMontage*> AttackMontages;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess="true"),Category = "Montage")
	TArray<UAnimMontage*> DeathMontages;

	//end animation
	


	
	
	void SetHealth(float NewHealth);

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bDebugMode;
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetHealth() const {return Health;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetArmor() const {return Armor;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetAttackDamage() const {return AttackDamage;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetAttackSpeed() const {return AttackSpeed;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMoveSpeed() const {return MoveSpeed;}
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetGoldCost() const {return GoldCost;}
};

