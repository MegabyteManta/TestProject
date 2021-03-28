// Fill out your copyright notice in the Description page of Project Settings.

#include "Boid.h"

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	Position = GetActorLocation();
	CachedPosition = Position;
	Forward = GetActorForwardVector();
	CachedForward = Forward;
	
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DTime = DeltaTime;
}

void ABoid::Initialize(FBoidSettings BoidSettings, AActor* BoidTarget) {
	Settings = BoidSettings;
	Target = BoidTarget;

	Position = CachedPosition;
	Forward = CachedForward;

	float StartSpeed = (Settings.MinSpeed + Settings.MaxSpeed) / 2;
	Velocity = GetActorForwardVector() * StartSpeed;
}

void ABoid::UpdateBoid() {
	Acceleration = FVector::ZeroVector;
	//UE_LOG(LogTemp, Warning, TEXT("ACCEL0: %s"), *Acceleration.ToString());

	if (Target != nullptr) {
		FVector OffsetToTarget = (Target->GetActorLocation() - Position);
		

		Acceleration = SteerTowards(OffsetToTarget) * Settings.TargetWeight;
		//UE_LOG(LogTemp, Warning, TEXT("ACCEL1: %s"), *Acceleration.ToString());

	}

	if (NumPerceivedFlockmates != 0) {
		CenterOfFlockmates /= NumPerceivedFlockmates;

		FVector OffsetToFlockmatesCenter = CenterOfFlockmates - Position;

		FVector AlignmentForce = SteerTowards(AvgFlockHeading) * Settings.AlignWeight;
		FVector CohesionForce = SteerTowards(OffsetToFlockmatesCenter) * Settings.CohesionWeight;
		FVector SeparationForce = SteerTowards(AvgAvoidanceHeading) * Settings.SeperateWeight;

		Acceleration += AlignmentForce;
		Acceleration += CohesionForce;
		Acceleration += SeparationForce;
		//UE_LOG(LogTemp, Warning, TEXT("AlignmentForce: %s"), *AlignmentForce.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("CohesionForce: %s"), *CohesionForce.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("SeparationForce: %s"), *SeparationForce.ToString());

	}

	
	if (IsHeadingForCollision()) {
		FVector CollisionAvoidDir = ObstacleRays();
		FVector CollisionAvoidForce = SteerTowards(CollisionAvoidDir) * Settings.AvoidCollisionWeight;
		Acceleration += CollisionAvoidForce;
		count++;
		UE_LOG(LogTemp, Warning, TEXT("COLLIDING: %d"), count);

	}
	
	
	
	
	Velocity += Acceleration * DTime;
	float Speed = Velocity.Size();
	FVector Dir = Velocity / Speed;
	Speed = FMath::Clamp(Speed, Settings.MinSpeed, Settings.MaxSpeed);
	Velocity = Dir * Speed;

	CachedPosition += Velocity * DTime;
	CachedForward = Dir;
	Position = FMath::VInterpTo(GetActorLocation(), CachedPosition, DTime, 1.f);
	SetActorLocation(Position);
	//CachedForward = Position;
	Forward = CachedForward;
	//SetActorRotation(Forward.Rotation());
	//UE_LOG(LogTemp, Warning, TEXT("MOVING: %s"), *Position.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("VELOCITY: %s"), *Velocity.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("SPEED: %f"), Speed);

}

bool ABoid::IsHeadingForCollision() {
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Settings.BoundsRadius); // 5M Radius
	FHitResult OutResult;
	ECollisionChannel TraceChannel = ECC_WorldStatic;
	FVector SweepStart = GetActorLocation();
	FVector SweepEnd = GetActorLocation() + GetActorForwardVector()*Settings.CollisionAvoidDst; //YOU PROBS NEED TO ADD THE ACTOR LOCAtION
	FCollisionQueryParams CollisionQueryParams;
	return GetWorld()->SweepSingleByChannel(OutResult, SweepStart, SweepEnd, FQuat::Identity, TraceChannel, Sphere);
}

FVector ABoid::ObstacleRays() {
	TArray<FVector> RayDirections = GetRayDirections();

	for (int i = 0; i < RayDirections.Num(); i++) {
		//HEREEEEE, YOU DON"T HAVE TO BE MAKING ALL THESE EVERY FRAME!!!!!!!!!!
		//FVector Dir = RayDirections[i] + CachedPosition;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(Settings.BoundsRadius); // 5M Radius
		FHitResult OutResult;
		ECollisionChannel TraceChannel = ECC_Pawn;
		FVector SweepStart = GetActorLocation();
		FVector SweepEnd = GetActorLocation() + RayDirections[i] * Settings.CollisionAvoidDst; //THIS MIGHT BE WRONG
		
		if (GetWorld()->SweepSingleByChannel(OutResult, SweepStart, SweepEnd, FQuat::Identity, TraceChannel, Sphere)) {
			return RayDirections[i];
		}
		
	}
	return Forward;
}

TArray<FVector> ABoid::GetRayDirections() {
	TArray<FVector> Directions;

	float GoldenRatio = (1 + FMath::Sqrt(5)) / 2;
	float AngleIncrement = 2 * PI * GoldenRatio;
	for (int i = 0; i < Settings.NumViewDirections; i++) {
		float T = (float)i / Settings.NumViewDirections;
		float Phi = FMath::Acos(1-2*T);
		float Theta = AngleIncrement * i;
		
		float X = FMath::Sin(Phi) * FMath::Cos(Theta);
		float Y = FMath::Sin(Phi) * FMath::Sin(Theta);
		float Z = FMath::Cos(Phi);
		Directions.Add(FVector(X, Y, Z).GetSafeNormal());
	}
	return Directions;
}

FVector ABoid::SteerTowards(FVector Vector) {
	FVector V = Vector.GetSafeNormal() * Settings.MaxSpeed - Velocity;
	//UE_LOG(LogTemp, Warning, TEXT("V: %s"), *Vector.ToString());
	return V.GetClampedToMaxSize(Settings.MaxSteerForce);
}
