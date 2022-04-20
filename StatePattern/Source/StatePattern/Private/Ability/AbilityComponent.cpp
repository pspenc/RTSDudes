// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/AbilityComponent.h"
#include "Ability/Ability.h"
#include "Unit/Unit.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

UAbilityComponent* UAbilityComponent::GetAbilityComponentFromActor(AActor* inActor)
{
	if(inActor)
	{
		UAbilityComponent* Comp = inActor->FindComponentByClass<UAbilityComponent>();
		return Comp;
	}
	return nullptr;
}

bool UAbilityComponent::ActivateAbility(UAbility* AbilityToActive,AActor* OptionalTarget)
{
	if(!GetOwner()){return false;}	//missing actor ref
	if(!GetOwningUnit()->CanUseAbilities()){return false;} //Unit cant use abilities, dont bother checking further  
	if(OwnedAbilities.Contains(AbilityToActive))
	{
		if(AbilityToActive->CanActivateAbility())
		{
			AbilityToActive->SetAbilityTarget(OptionalTarget);
			return AbilityToActive->ActivateAbility();
		}
	}
	return false;
}

bool UAbilityComponent::ActivateAbilityByName(FName AbilityName)
{
	
	if(!GetOwner()){return false;}	//missing actor ref
	if(!GetOwningUnit()->CanUseAbilities()){return false;} //Unit cant use abilities, dont bother checking further

	for(auto Ability : OwnedAbilities)
	{
		if(Ability->AbilityName == AbilityName)
		{
			return ActivateAbility(Ability);
			
		}
	}

	return false;
	
}

UAbility* UAbilityComponent::GetAbilityByName(FName AbilityName)
{
	for(auto Ability : OwnedAbilities)
	{
		if(Ability->AbilityName == AbilityName)
		{
			return Ability;
			
		}
	}
	return nullptr;
}

UAbility* UAbilityComponent::GrantAbility(TSubclassOf<UAbility> AbilityClass,int Level)
{
	if(!AbilityClass){return false;}

	UAbility* NewAbility = NewObject<UAbility>(GetOwner(),AbilityClass,AbilityClass.GetDefaultObject()->AbilityName);
	NewAbility->Level = Level;
	OwnedAbilities.Add(NewAbility);

	return NewAbility;
}

bool UAbilityComponent::RemoveAbilityByName(FName AbilityName)
{
	
	if(!GetOwner()){return false;}	//missing actor ref
	
	for(auto Ability : OwnedAbilities)
	{
		if(Ability->AbilityName == AbilityName)
		{
			OwnedAbilities.Remove(Ability);
			return true;
			
		}
	}
	return false;
}

bool UAbilityComponent::RemoveAbility(UAbility* Ability)
{
	if(!Ability){return false;}

	if(OwnedAbilities.Contains(Ability))
	{
		if(Ability->IsActive())
		{
			Ability->EndAbility();
		}
		OwnedAbilities.Remove(Ability);
		return true;
	}
	
	return false;
}


AUnit* UAbilityComponent::GetOwningUnit()
{
	if(OwningUnit) //if our casted owner ref is valid
	{
		return OwningUnit;
	}
	if(GetOwner())
	{
		AUnit* CastedRef = Cast<AUnit>(GetOwner());
		if(CastedRef)
		{
			InitOwnerProperties(CastedRef);
			return OwningUnit;
		}
	}
	return nullptr;
}
void UAbilityComponent::InitOwnerProperties(class AUnit* NewOwner)
{
	if(!NewOwner){return;}
	if(NewOwner == OwningUnit){return;}//already our dude
	OwningUnit = NewOwner;
	
}

// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwningUnit();
	GrantDefaultAbilities();
	// ...
	
}

void UAbilityComponent::GrantDefaultAbilities()
{
	if(bHasGrantedDefaultAbilities){return;}

	for(auto Ability : GetOwningUnit()->DefaultAbilities)
	{
		GrantAbility(Ability,1);
	}
	bHasGrantedDefaultAbilities = true;
}



