// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidManager.generated.h"

class ABoid;

USTRUCT(BlueprintType)
struct TESTPROJECT_API FBoidSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PerceptionRadius = 2.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvoidanceRadius = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSteerForce = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AlignWeight = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CohesionWeight = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SeperateWeight = 1;

	UPROPERTY(EditAnywhere)
	float TargetWeight = 1;

	//[Header("Collisions")]
	//LayerMask obstacleMask;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumViewDirections = 300;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoundsRadius = 27.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvoidCollisionWeight = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CollisionAvoidDst = 500.f;
};

UCLASS()
class TESTPROJECT_API ABoidManager : public AActor
{
	GENERATED_BODY()
		
	TArray<ABoid*> Boids;
	
public:	
	// Sets default values for this actor's properties
	ABoidManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoidSettings")
		FBoidSettings Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoidSpawning")
		TSubclassOf<ABoid> BoidPrefab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoidSpawning")
		int SpawnCount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoidSpawning")
		float SpawnRadius = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoidSpawning")
		AActor* Target;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetRandomPointInSphere(FVector Center, float Radius);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<ABoid*> SpawnBoids(TSubclassOf<ABoid> Prefab, int Count, FVector Location, float Radius);



};
