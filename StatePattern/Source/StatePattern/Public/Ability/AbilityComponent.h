// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STATEPATTERN_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();

	UFUNCTION(BlueprintPure)
	static UAbilityComponent* GetAbilityComponentFromActor(AActor* inActor);



	UFUNCTION(BlueprintCallable)
	bool ActivateAbility(class UAbility* AbilityToActive,AActor* OptionalTarget = nullptr);
	UFUNCTION(BlueprintCallable)
	bool ActivateAbilityByName(FName AbilityName);

	UFUNCTION(BlueprintPure)
	UAbility* GetAbilityByName(FName AbilityName);

	UFUNCTION(BlueprintCallable)
	UAbility* GrantAbility(TSubclassOf<UAbility> AbilityClass,int Level);


	UFUNCTION(BlueprintCallable)
	bool RemoveAbilityByName(FName UnitName);
	UFUNCTION(BlueprintCallable)
	bool RemoveAbility(UAbility* Ability);
	
	bool bHasGrantedDefaultAbilities;

	UFUNCTION(BlueprintPure)
	class AUnit* GetOwningUnit();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void GrantDefaultAbilities();

	
	UPROPERTY(BlueprintReadOnly,VisibleInstanceOnly)
	TArray<class UAbility*> OwnedAbilities;

	UPROPERTY()
	class AUnit* OwningUnit;

	void InitOwnerProperties(class AUnit* NewOwner);
	
public:	

		
};
