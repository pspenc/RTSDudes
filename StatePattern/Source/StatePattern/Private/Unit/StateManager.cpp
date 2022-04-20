// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/StateManager.h"

#include "UnitStateInterface.h"
#include "StatePattern/StatePattern.h"

// Sets default values for this component's properties
UStateManager::UStateManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// ...
}

bool UStateManager::ActionsAllowed()
{
	for(auto State : ActiveStates)
	{
		if(State)
		{	
			if(State->BlocksActions())
			{
				return false;
			}
		}
	}
	return true;
}

bool UStateManager::MovementAllowed()
{
	for(auto State : ActiveStates)
    {
    	if(State)
    	{	
    		if(State->BlocksMovement())
    		{
    			return false;
    		}
    	}
    }
    return true;
}

bool UStateManager::AnimationResourceAvailable()
{
	return AnimationResourceHolder ? false : true;
}

bool UStateManager::ActionResourceAvailable()
{
	return ActionResourceHolder ? false : true;
}

bool UStateManager::MovementResourceAvailable()
{
	return MovementResourceHolder ? false : true;
}

bool UStateManager::ClaimAnimationResource(TScriptInterface<IUnitStateInterface> ClaimantState)
{
	if(AnimationResourceAvailable())
	{
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Animation Resource claimed by %s"),*GetName(),*GetOwner()->GetName(),*ClaimantState.GetObject()->GetName())
		AnimationResourceHolder = ClaimantState;
		return true;
	}
	else if(AnimationResourceHolder == ClaimantState){return true;}
	return false;
}

bool UStateManager::ClaimActionResource(TScriptInterface<IUnitStateInterface> ClaimantState)
{
	if(ActionResourceAvailable())
    {
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Action Resource claimed by %s"),*GetName(),*GetOwner()->GetName(),*ClaimantState.GetObject()->GetName())
    	ActionResourceHolder = ClaimantState;
		return true;
    }else if(ActionResourceHolder == ClaimantState){return true;}
    return false;
}

bool UStateManager::ClaimMovementResource(TScriptInterface<IUnitStateInterface> ClaimantState)
{
	if(MovementResourceAvailable())
	{
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Movement Resource claimed by %s"),*GetName(),*GetOwner()->GetName(),*ClaimantState.GetObject()->GetName())
		MovementResourceHolder = ClaimantState;
		return true;
	}else if(MovementResourceHolder == ClaimantState){return true;}
	return false;
}

void UStateManager::ReturnAnimationResource(TScriptInterface<IUnitStateInterface> ReturningState)
{
	if(ReturningState == AnimationResourceHolder)
	{
		AnimationResourceHolder.SetObject(nullptr);
		
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Animation Resource returned by %s"),*GetName(),*GetOwner()->GetName(),*ReturningState.GetObject()->GetName())

		for(auto State : ActiveStates)
		{
			//if state is waiting on resource, try having them claim it, if success, return
			if(State->IsWaitingOnAnimationResource() && State->TryClaimAnimationResource())
			{
				bActiveListLocked = false;
				return;
			}
		}
		bActiveListLocked = false;
	}
}

void UStateManager::ReturnActionResource(TScriptInterface<IUnitStateInterface> ReturningState)
{
	if(ReturningState == ActionResourceHolder)
	{
		ActionResourceHolder.SetObject(nullptr);
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Action Resource returned by %s"),*GetName(),*GetOwner()->GetName(),*ReturningState.GetObject()->GetName())
		for(auto State : ActiveStates)
		{
			
			//if state is waiting on resource, try having them claim it, if success, return
			if(State->IsWaitingOnActionResource() && State->TryClaimActionResource())
			{
				return;
			}
		}
	}
}

void UStateManager::ReturnMovementResource(TScriptInterface<IUnitStateInterface> ReturningState)
{
	if(ReturningState == MovementResourceHolder)
	{
		MovementResourceHolder.SetObject(nullptr);
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Movement Resource returned by %s"),*GetName(),*GetOwner()->GetName(),*ReturningState.GetObject()->GetName())
		for(auto State : ActiveStates)
		{
			//if state is waiting on resource, try having them claim it, if success, return
			if(State->IsWaitingOnMoveResource() && State->TryClaimMoveResource())
			{
				return;
			}
		}
	}
}


void UStateManager::AddActiveState(TScriptInterface<IUnitStateInterface> State)
{
	if(bActiveListLocked)
	{
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Added new active state : %s"),*GetName(),*GetOwner()->GetName(),*State.GetObject()->GetName())
		NextFrameStatesToAdd.Add(State);
		return;
	}
	ActiveStates.Add(State);
}

void UStateManager::RemoveActiveState(TScriptInterface<IUnitStateInterface> State)
{
	//if this state is has one of our resources, have them return it before removing
	if(State == MovementResourceHolder){ReturnMovementResource(State);}
	if(State == ActionResourceHolder){ReturnActionResource(State);}
	if(State == AnimationResourceHolder){ReturnAnimationResource(State);}
	UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Adding %s to pending removal"),*GetName(),*GetOwner()->GetName(),*State.GetObject()->GetName())
	StatesToRemove.Add(State);
	
}


// Called when the game starts
void UStateManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStateManager::CheckForWaitingStates()
{
	bActiveListLocked = true;
	for(auto State : ActiveStates)
	{
		if(ActionResourceAvailable())
		{
			//if state is waiting on resource, try having them claim it, if success, return
			if(State->IsWaitingOnActionResource())
			{
				State->TryClaimActionResource();
			}
		}
		if(AnimationResourceAvailable())
		{
			//if state is waiting on resource, try having them claim it, if success, return
			if(State->IsWaitingOnAnimationResource() )
			{
				State->TryClaimAnimationResource();
			}
		}
		if(MovementResourceAvailable())
		{
			//if state is waiting on resource, try having them claim it, if success, return
			if(State->IsWaitingOnMoveResource() )
			{
				State->TryClaimMoveResource();
			}
		}
	}
	bActiveListLocked = false;
}

void UStateManager::CleanupRemovedStates()
{
	bActiveListLocked = true;
	for(auto RemoveState : StatesToRemove)
	{
		UE_LOG(LogStateSystem,Display,TEXT("%s on %s: Removing %s state"),*GetName(),*GetOwner()->GetName(),*RemoveState.GetObject()->GetName())
		ActiveStates.Remove(RemoveState);
	}
	bActiveListLocked = false;
	StatesToRemove.Empty();
}


// Called every frame
void UStateManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckForWaitingStates();
	CleanupRemovedStates();
	
	if(!bActiveListLocked)
	{
		for(auto State : NextFrameStatesToAdd)
		{
			AddActiveState(State);
		}
	}
	// ...
}

