// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward/RewardInstance.h"

URewardInstance::URewardInstance()
{
	Type = ERewardType::UnitProduction;
	Rarity = ERewardRarity::Common;
	RewardID = "PeasantProduction";
	RewardTarget = "Peasant";
	Factor = 1;
}
