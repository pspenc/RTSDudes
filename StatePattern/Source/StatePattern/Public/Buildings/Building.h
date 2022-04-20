// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UCLASS()
class STATEPATTERN_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

#if WITH_EDITOR	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	
	#endif	


	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float BuildingWidth = 3;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float BuildingDepth = 3;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float BuildingHeight = 2;



	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* BuildingMesh;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UBoxComponent* BuildingCollision;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UBoxComponent* ConstructionPreventionArea;

	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bDebugMode;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CurrentHealth;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Armor;


	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetupCollisionBoxes();


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float BuildingUnitSize = 200;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
