// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitPlayerController.h"

#include "Ability/Ability.h"
#include "Ability/AbilityComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Unit/PlayerUnit.h"

#include "UI/UI_InterWave.h"
#include "UI/UI_GameHUD.h"
#include "Unit/StateManager.h"
#include "Unit/Unit.h"


AUnitPlayerController::AUnitPlayerController()
{
	Ability1Name = "Empty";
	Ability2Name = "Empty";
	Ability3Name = "Empty";

	CameraPitchBounds = FVector2D(-60,10);
}

void AUnitPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledUnit = Cast<APlayerUnit>(InPawn);
	if(ControlledUnit)
	{
		ControlledUnit->OnHealthChange.AddDynamic(this,&AUnitPlayerController::OnUnitHealthChanged);
	}
	if(ControlledUnit->AbilityComponent)
	{
		if(AbilityToImpMappingTable)
		{
			if(Ability1Name != "Empty")
			{
				FAbilityDataToImpMapping* DataPtr = AbilityToImpMappingTable->FindRow<FAbilityDataToImpMapping>(Ability1Name,"GettingRewardImpClass");
				if(DataPtr)
				{
					
					Ability1 = ControlledUnit->AbilityComponent->GrantAbility(DataPtr->ImplementationClass,Ability1Level);
					
				}
			}
			if(Ability2Name != "Empty")
			{
				FAbilityDataToImpMapping* DataPtr = AbilityToImpMappingTable->FindRow<FAbilityDataToImpMapping>(Ability2Name,"GettingRewardImpClass");
				if(DataPtr)
				{
					Ability2 =ControlledUnit->AbilityComponent->GrantAbility(DataPtr->ImplementationClass,Ability2Level);
				}
			}
			if(Ability3Name != "Empty")
			{
				FAbilityDataToImpMapping* DataPtr = AbilityToImpMappingTable->FindRow<FAbilityDataToImpMapping>(Ability3Name,"GettingRewardImpClass");
				if(DataPtr)
				{
					Ability3 =ControlledUnit->AbilityComponent->GrantAbility(DataPtr->ImplementationClass,Ability3Level);
				}
			}
			
		}
	}
}

void AUnitPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("Jump", IE_Pressed, this, &AUnitPlayerController::TryJump);
	InputComponent->BindAction("Attack", IE_Pressed, this, &AUnitPlayerController::TryAttack);
	InputComponent->BindAction("Ability1", IE_Pressed, this, &AUnitPlayerController::UseAbility1);
	InputComponent->BindAction("Ability2", IE_Pressed, this, &AUnitPlayerController::UseAbility2);
	InputComponent->BindAction("Ability3", IE_Pressed, this, &AUnitPlayerController::UseAbility3);
	

	InputComponent->BindAxis("MoveForward", this, &AUnitPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUnitPlayerController::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &AUnitPlayerController::AddYawInput);
	InputComponent->BindAxis("TurnRate", this, &AUnitPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AUnitPlayerController::AddPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AUnitPlayerController::LookUpAtRate);
}

void AUnitPlayerController::TryJump()
{
	if(ControlledUnit)
	{
		ControlledUnit->Jump();
	}
}

void AUnitPlayerController::TryAttack()
{
	if(ControlledUnit && ControlledUnit->CanBasicAttack())
	{
		ControlledUnit->Attack(nullptr);
	}
}

void AUnitPlayerController::UseAbility1()
{
	if(Ability1)
	{
		UseAbility(1);
	}
}

void AUnitPlayerController::UseAbility2()
{
	if(Ability2)
	{
		UseAbility(2);
	}
}

void AUnitPlayerController::UseAbility3()
{
	if(Ability3)
	{
		UseAbility(3);
	}
}

void AUnitPlayerController::SwapToSpectatorMode()
{
	if(GameHUD)
	{
		GameHUD->RemoveFromParent();
		GameHUD = nullptr;
	}
	if(GetPawn())
	{
		GetPawn()->Destroy();
	}
	SpawnSpectatorPawn();
}

void AUnitPlayerController::ShowLoseScreen_Implementation()
{
	
}

void AUnitPlayerController::ShowWinScreen_Implementation()
{
}

void AUnitPlayerController::GiveChoosableRewards(TArray<URewardInstance*> Rewards)
{
	ChoosableRewards = Rewards;
	if(InterWaveHUD)
	{
		InterWaveHUD->DisplayRewardSelection();
	}
}

void AUnitPlayerController::SelectUpgrade(URewardInstance* SelectedReward)
{
	
	
	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>( UGameplayStatics::GetGameMode(this));
	
	if(GM)
	{
		GM->ConfirmUpgrade(SelectedReward);
	}
	
	ChoosableRewards.Empty();
}

void AUnitPlayerController::UseAbility(int Index)
{
	//if no pawn, do nothing
	if(!GetPawn()){return;}


	UAbility* AbilityToActivate;
	switch(Index)
	{
		case 1:
			AbilityToActivate = Ability1;
		break;
		case 2:
			AbilityToActivate = Ability2;
			break;
		case 3:
			AbilityToActivate = Ability3;
		break;
		default:
			AbilityToActivate = Ability1;
		break;
	}
	if(!AbilityToActivate){return;}

	//pass it off to the AbilityComponent, and let it do additional checks
	UAbilityComponent* AbilityComponent = GetPawn()->FindComponentByClass<UAbilityComponent>();
	if(AbilityComponent)
	{
		AbilityComponent->ActivateAbility(AbilityToActivate);
	}
	
}

bool AUnitPlayerController::AbilitySlotEmpty(int Index) const
{
	if(Index == 1 && Ability1Name == "Empty"){return true;}
	if(Index == 2 && Ability2Name == "Empty"){return true;}
	if(Index == 3 && Ability3Name == "Empty"){return true;}
	return false;
}

int AUnitPlayerController::HasAbility(FName AbilityName)
{
	if(AbilityName == Ability1Name)
	{
		return 0;
	}
	if(AbilityName == Ability2Name)
	{
		return 1;
	}
	if(AbilityName == Ability3Name)
	{
		return 2;
	}
	return -1;
}


void AUnitPlayerController::GrantAbility(FName AbilityName,int AbilityLevel)
{
	PresentSlotSelection(AbilityName,AbilityLevel);
	return;
	int SlotToUse = 1;
	bool FoundEmpty = false;
	for(int i = 0; i <=3 ;i++)
	{
		if(AbilitySlotEmpty(i))
		{
			SlotToUse = i;
			FoundEmpty = true;
			break;
		}
	}
	if(FoundEmpty)
	{		
		if(SlotToUse == 1)	//omg this is gross
		{
			Ability1Level = AbilityLevel;
			Ability1Name = AbilityName;
			
		}
		else if(SlotToUse == 2)
		{
			Ability2Level = AbilityLevel;
			Ability2Name = AbilityName;
			
		}
		else if(SlotToUse == 3)
		{
			Ability3Level = AbilityLevel;
			Ability3Name = AbilityName;
			
		}
			
			
		OnAbilitiesChanged.Broadcast();
	}
	
}
void AUnitPlayerController::AssignSlot(FName AbilityName,int Level, int Slot)
{
	switch(Slot)
	{
		case 0: Ability1Level = Level;
				Ability1Name = AbilityName;
		break;
		case 1: Ability2Level = Level;
		Ability2Name = AbilityName;
		break;
		case 2: Ability3Level = Level;
		Ability3Name = AbilityName;
		break;
			default: ;
	}
	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>( UGameplayStatics::GetGameMode(this));
	
	if(GM)
	{
		
	}
	OnAbilitiesChanged.Broadcast();
	OnAbilitiesChanged.Clear();
}
void AUnitPlayerController::UpgradeAbility(FName AbilityName, int LevelsToIncreaseBy)
{
	int IndexSlot = HasAbility(AbilityName);
	if(IndexSlot != -1)
	{
		switch(IndexSlot)
		{
			case 0 : Ability1Level +=LevelsToIncreaseBy;
			break;
		case 1 : Ability2Level +=LevelsToIncreaseBy;
			break;
		case 2 : Ability3Level +=LevelsToIncreaseBy;
			break;
		}
	}
}

void AUnitPlayerController::StartWaveCountdown_Implementation(float Time)
{
	if(GameHUD)
	{
		GameHUD->StartWaveCountdown(Time);
	}
}

void AUnitPlayerController::DisplayInterWaveHUD_Implementation()
{

	if(GameHUD)
	{
		GameHUD->RemoveFromParent();
		GameHUD = nullptr;
	}
	InterWaveHUD = Cast<UUI_InterWave>(CreateWidget(this,InterWaveHUDClass));
	if(InterWaveHUD)
	{
		InterWaveHUD->AddToViewport();
		SetShowMouseCursor(true);
		SetInputMode(FInputModeUIOnly());
	}
	
}

void AUnitPlayerController::DisplayGameHUD_Implementation()
{
	if(InterWaveHUD)
	{
		InterWaveHUD->RemoveFromParent();
		InterWaveHUD = nullptr;
	}
	GameHUD = Cast<UUI_GameHUD>(CreateWidget(this,GameHUDClass));
	if(GameHUD)
	{
		GameHUD->AddToViewport();
		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());
	}
	
}


void AUnitPlayerController::OnUnitHealthChanged(float NewHealth)
{
	if(GameHUD)
	{
		GameHUD->UpdateHealth(NewHealth);
	}
}


void AUnitPlayerController::MoveForward(float Value)
{
	if(ControlledUnit && ControlledUnit->StateManager->MovementAllowed())
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		ControlledUnit->AddMovementInput(Direction,Value);
	
		
	}
	
}



void AUnitPlayerController::MoveRight(float Value)
{
	if(ControlledUnit && ControlledUnit->StateManager->MovementAllowed())
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		ControlledUnit->AddMovementInput(Direction,Value);
	
		
		
	}
}



void AUnitPlayerController::TurnAtRate(float Rate)
{
	if(!ControlledUnit){return;}
	/*
	if(Rate > 0 && ControlledUnit->SpringArm->GetRelativeRotation().Yaw > CameraYawBounds.X)
	{
		return;
	}
	else if(Rate < 0 && ControlledUnit->SpringArm->GetRelativeRotation().Yaw < CameraYawBounds.Y)
	{
		return;
	}
	*/
	AddYawInput(Rate * 45.f * GetWorld()->GetDeltaSeconds());
}

void AUnitPlayerController::Turn(float Value)
{
	if(!ControlledUnit){return;}

	/*
	if(Value > 0 && ControlledUnit->SpringArm->GetRelativeRotation().Yaw > CameraYawBounds.X)
	{
		return;
	}
	else if(Value < 0 && ControlledUnit->SpringArm->GetRelativeRotation().Yaw < CameraYawBounds.Y)
	{
		return;
	}
	*/
	//if not out of a bounds, do dis.
	AddYawInput(Value);
}

void AUnitPlayerController::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if(!ControlledUnit){return;}
	if(Rate > 0 && ControlledUnit->SpringArm->GetRelativeRotation().Pitch > CameraPitchBounds.Y)
	{
		return;
	}
	else if(Rate < 0 && ControlledUnit->SpringArm->GetRelativeRotation().Pitch < CameraPitchBounds.X)
	{
		return;
	}
	AddPitchInput(Rate * 45.f * GetWorld()->GetDeltaSeconds());
}

void AUnitPlayerController::LookUp(float Value)
{
	if(!ControlledUnit){return;}
	if(Value > 0 && ControlledUnit->SpringArm->GetRelativeRotation().Pitch > CameraPitchBounds.Y)
	{
		return;
	}
	else if(Value < 0 && ControlledUnit->SpringArm->GetRelativeRotation().Pitch < CameraPitchBounds.X)
	{
		return;
	}
	//if not out of a bounds, do dis.
	AddPitchInput(Value);
}

