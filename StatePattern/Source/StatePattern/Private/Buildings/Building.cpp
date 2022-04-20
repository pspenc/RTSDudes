// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/Building.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>("BuildingMesh");
	//BuildingMesh->SetupAttachment(BuildingCollision);
	SetRootComponent(BuildingMesh);

	BuildingCollision = CreateDefaultSubobject<UBoxComponent>("BuildingCollision");
	BuildingCollision->SetupAttachment(BuildingMesh);

	ConstructionPreventionArea = CreateDefaultSubobject<UBoxComponent>("ConstructionPreventionArea");
	ConstructionPreventionArea->SetupAttachment(BuildingMesh);

	SetupCollisionBoxes();

	
	
}
#if WITH_EDITOR

void ABuilding::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SetupCollisionBoxes();

}
#endif
void ABuilding::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BuildingMesh->SetRelativeRotation(FRotator(0,-180,0));
	SetupCollisionBoxes();
}


// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABuilding::SetupCollisionBoxes()
{
	if(BuildingCollision)
	{
		BuildingCollision->SetBoxExtent(FVector((BuildingUnitSize/2)*BuildingWidth,(BuildingUnitSize/2)*BuildingDepth,(BuildingUnitSize/2)*BuildingHeight));
		BuildingCollision->SetRelativeLocation(FVector(0,0,BuildingUnitSize * (BuildingHeight/2)));
	}
	if(ConstructionPreventionArea)
	{
		ConstructionPreventionArea->SetBoxExtent(FVector((BuildingUnitSize/1.5) * BuildingWidth,(BuildingUnitSize/1.5)*BuildingDepth,BuildingUnitSize/4));

		ConstructionPreventionArea->SetRelativeLocation(FVector(0,0,BuildingUnitSize/8));
	}
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

