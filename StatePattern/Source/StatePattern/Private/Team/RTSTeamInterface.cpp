// Fill out your copyright notice in the Description page of Project Settings.


#include "Team/RTSTeamInterface.h"

ETeam IRTSTeamInterface::GetActorsTeam(const AActor* TeamMember)
{
	const IRTSTeamInterface* TeamAgent = Cast<const IRTSTeamInterface>(TeamMember);
	if (TeamAgent)
	{
		return TeamAgent->GetTeam();
	}

	return ETeam::Neutral;
}

// Add default functionality here for any IRTSTeamInterface functions that are not pure virtual.
ETeamResponse IRTSTeamInterface::GetTeamResponse( AActor* A,  AActor* B)
{
	 IRTSTeamInterface* TeamAgentA = Cast< IRTSTeamInterface>(A);

	return TeamAgentA == nullptr || B == nullptr ? ETeamResponse::Neutral : TeamAgentA->GetTeamResponseTo(B);
}

ETeamResponse IRTSTeamInterface::GetTeamResponse(ETeam TeamA, ETeam TeamB)
{
	if(TeamA == ETeam::Neutral ){return ETeamResponse::Neutral;}
	if(TeamB == ETeam::Neutral){return ETeamResponse::Neutral;}

	return TeamA != TeamB ? ETeamResponse::Hostile : ETeamResponse::Friendly;
}
