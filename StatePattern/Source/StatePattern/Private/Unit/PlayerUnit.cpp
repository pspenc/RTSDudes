// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/PlayerUnit.h"

#include "Kismet/GameplayStatics.h"


APlayerUnit::APlayerUnit()
{

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
}




void APlayerUnit::FinishDying()
{
	ALaneTSGameMode* GM = Cast<ALaneTSGameMode>(UGameplayStatics::GetGameMode(this));
	if(GM)
	{
		GM->PlayerDied();
	}
}
