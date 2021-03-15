// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoidManager.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class TESTPROJECT_API ABoid : public AActor
{
	GENERATED_BODY()

	float DTime = 0;

	FVector Velocity = FVector::ZeroVector;
	FVector Acceleration = FVector::ZeroVector;

	//Cached
	FVector CachedPosition;
	FVector CachedForward;
	AActor* Target;

public:	
	// Sets default values for this actor's properties
	ABoid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBoidSettings Settings;

	FVector Position;
	FVector Forward;

	FVector AvgFlockHeading = FVector::ZeroVector;
	FVector AvgAvoidanceHeading = FVector::ZeroVector;
	FVector CenterOfFlockmates = FVector::ZeroVector;
	int NumPerceivedFlockmates = 0;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsHeadingForCollision();
	FVector ObstacleRays();
	TArray<FVector> GetRayDirections();
	FVector SteerTowards(FVector Vector);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	void Initialize(FBoidSettings BoidSettings, AActor* BoidTarget);
	//UFUNCTION()
	void UpdateBoid();


};
