// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitStateInterface.h"

#include "Unit/StateManager.h"

bool IUnitStateInterface::BlocksMovement()
{
	return false;
}

bool IUnitStateInterface::BlocksActions()
{
	return false;
}



bool IUnitStateInterface::IsWaitingOnMoveResource()
{
	return false;
}

bool IUnitStateInterface::IsWaitingOnActionResource()
{
	return false;
}

bool IUnitStateInterface::IsWaitingOnAnimationResource()
{
	return false;
}
