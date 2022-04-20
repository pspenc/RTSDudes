// Fill out your copyright notice in the Description page of Project Settings.



#include "AdvObject/AdvancedObject.h"


void UAdvancedObject::PostInitProperties()
{
	Super::PostInitProperties();
	if(GetOuter() && GetOuter()->GetWorld())
	{
		NativeBeginPlay();
		BeginPlay();
	}
}

void UAdvancedObject::NativeBeginPlay()
{
}


UWorld* UAdvancedObject::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld) return nullptr; 
	else if (GetOuter()) return GetOuter()->GetWorld(); 
	else return nullptr;
}
AActor* UAdvancedObject::GetOwningActor() const
{
	return GetTypedOuter<AActor>();
}
void UAdvancedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// Add any Blueprint properties
	// This is not required if you do not want the class to be "Blueprintable"
	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool UAdvancedObject::IsSupportedForNetworking() const
{
	return true;
}
int32 UAdvancedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}
bool UAdvancedObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	AActor* Owner = GetOwningActor();
	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (NetDriver)
	{
		NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}
void UAdvancedObject::Destroy()
{
	if (!IsPendingKill())
	{
		checkf(GetOwningActor()->HasAuthority() == true, TEXT("Destroy:: Object does not have authority to destroy itself!"));
			
		OnDestroyed();
		MarkPendingKill();
	}
}

void UAdvancedObject::OnDestroyed()
{
	// Notify Owner etc.
}
