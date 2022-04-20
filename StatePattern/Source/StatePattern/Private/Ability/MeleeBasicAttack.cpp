// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/MeleeBasicAttack.h"

#include "Unit/Unit.h"

UMeleeBasicAttack::UMeleeBasicAttack()
{
	AbilityName = "BasicAttack";
}

void UMeleeBasicAttack::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if(OwningUnit)
	{
		bCleaves = OwningUnit->AttackCleaves;
		
	}
}
