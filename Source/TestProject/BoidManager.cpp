// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidManager.h"
#include "Boid.h"

// Sets default values
ABoidManager::ABoidManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoidManager::BeginPlay()
{
	Super::BeginPlay();
	Boids = SpawnBoids(BoidPrefab, SpawnCount, GetActorLocation(), SpawnRadius);
	
	
}

// Called every frame
void ABoidManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Boids.Num() != 0) {
		for (int i = 0; i < Boids.Num(); i++) {
			Boids[i]->NumPerceivedFlockmates = 0;
			Boids[i]->AvgFlockHeading = FVector::ZeroVector;
			Boids[i]->CenterOfFlockmates = FVector::ZeroVector;
			Boids[i]->AvgAvoidanceHeading = FVector::ZeroVector;
			/*
			UE_LOG(LogTemp, Warning, TEXT("NumPerceivedFlockmates: %d"), Boids[i]->NumPerceivedFlockmates);
			UE_LOG(LogTemp, Warning, TEXT("AvgFlockHeading: %s"), *Boids[i]->AvgFlockHeading.ToString());
			UE_LOG(LogTemp, Warning, TEXT("CenterOfFlockmates: %s"), *Boids[i]->CenterOfFlockmates.ToString());
			UE_LOG(LogTemp, Warning, TEXT("AvgAvoidanceHeading: %s"), *Boids[i]->AvgAvoidanceHeading.ToString());
			*/
			for (int j = 0; j < Boids.Num(); j++) {
				
				if (i == j) continue;
				FVector Offset = Boids[j]->Position - Boids[i]->Position;
				float SqrDst = Offset.X * Offset.X + Offset.Y * Offset.Y + Offset.Z * Offset.Z;

				if (SqrDst < Settings.PerceptionRadius * Settings.PerceptionRadius) {
					Boids[i]->NumPerceivedFlockmates += 1;
					Boids[i]->AvgFlockHeading += Boids[j]->Forward;
					Boids[i]->CenterOfFlockmates += Boids[j]->Position;

					if (SqrDst < Settings.SeperateWeight * Settings.SeperateWeight && SqrDst != 0) {
						Boids[i]->AvgAvoidanceHeading -= Offset / SqrDst;
					}

				}
				
			}
			/*
			UE_LOG(LogTemp, Warning, TEXT("NumPerceivedFlockmates: %d"), Boids[i]->NumPerceivedFlockmates);
			UE_LOG(LogTemp, Warning, TEXT("AvgFlockHeading: %s"), *Boids[i]->AvgFlockHeading.ToString());
			UE_LOG(LogTemp, Warning, TEXT("CenterOfFlockmates: %s"), *Boids[i]->CenterOfFlockmates.ToString());
			UE_LOG(LogTemp, Warning, TEXT("AvgAvoidanceHeading: %s"), *Boids[i]->AvgAvoidanceHeading.ToString());
			*/
			Boids[i]->UpdateBoid();
		}
	}

}

TArray<ABoid*> ABoidManager::SpawnBoids(TSubclassOf<ABoid> Prefab, int Count, FVector Location, float Radius) {
	TArray<ABoid*> SpawnedBoids;
	FActorSpawnParameters SpawnParams;
	for (int i = 0; i < SpawnCount; i++) {
		FVector Loc = GetRandomPointInSphere(Location, Radius);
		ABoid* Boid = GetWorld()->SpawnActor<ABoid>(Prefab, Loc, GetActorRotation(), SpawnParams);
		Boid->Initialize(Settings, Target);
		SpawnedBoids.Add(Boid);
	}
	return SpawnedBoids;
}

//Thanks https://datagenetics.com/blog/january32020/index.html
FVector ABoidManager::GetRandomPointInSphere(FVector Center, float Radius) {
	float Theta = FMath::RandRange(0.f, 2*PI);
	float V = FMath::RandRange(0.f, 1.f);
	float Phi = FMath::Acos((2 * V - 1));
	float R = Radius * FMath::Pow(FMath::RandRange(0.f, 1.f), 1.f / 3.f);
	
	float X = R * FMath::Sin(Phi) * FMath::Cos(Theta);
	float Y = R * FMath::Sin(Phi) * FMath::Sin(Theta);
	float Z = R * FMath::Cos(Phi);
	return Center + FVector(X, Y, Z);
}

