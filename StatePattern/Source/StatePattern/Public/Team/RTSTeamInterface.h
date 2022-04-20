// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenericTeamAgentInterface.h"
#include "UObject/Interface.h"
#include "RTSTeamInterface.generated.h"



UENUM(BlueprintType)
enum class ETeam : uint8
{
   
	Player,
	Enemy,
	Neutral
};
UENUM()
enum class ETeamResponse : uint8
{
   
	Hostile,
	Friendly,
	Neutral
};




// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URTSTeamInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STATEPATTERN_API IRTSTeamInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	
	static ETeam GetActorsTeam(const AActor* TeamMember);
	static ETeamResponse GetTeamResponse( AActor* A,  AActor* B);
	static ETeamResponse GetTeamResponse(ETeam TeamA,ETeam TeamB);

	virtual void SetTeam(ETeam NewTeam){};
	virtual ETeam GetTeam() const {return ETeam::Neutral;}
	/** Retrieved owner attitude toward given Other object */
	virtual ETeamResponse GetTeamResponseTo(const AActor* Other) const
	{ 
		const IRTSTeamInterface* OtherTeamAgent = Cast<const IRTSTeamInterface>(Other);
		return OtherTeamAgent ? GetTeamResponse(GetTeam(), OtherTeamAgent->GetTeam())
			: ETeamResponse::Neutral;
	}
	
};
