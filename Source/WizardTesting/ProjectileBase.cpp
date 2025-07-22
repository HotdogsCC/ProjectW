// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set it to replicate (so it works online)
	bReplicates = true;
    AActor::SetReplicateMovement(true);

	//mesh component set up
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(MeshComponent);
	

	//adds a light as a sub object
	LightComponent = CreateDefaultSubobject<UPointLightComponent>("Light");
	LightComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//has the projectile run out of time?
	if(DespawnTime <= 0.0f)
	{
		Destroy();
		return;
	}
	
	//decrement alive time
	DespawnTime -= DeltaTime;

	//move in its target direction
	FVector CurrentToTarget = TargetLocation - PreviousLocation;
	float CurDistanceToTarget = CurrentToTarget.Length();

	//its current percentage of travel, from 0-1
	float TravelCompletion = (InitDistance - CurDistanceToTarget) / InitDistance;
	
	
	FVector DirectionNormalised = CurrentToTarget.GetSafeNormal();
	FVector TravelVector = DirectionNormalised * MoveSpeed * DeltaTime;

	//are we going to overshoot?
	if(TravelVector.SquaredLength() > CurDistanceToTarget * CurDistanceToTarget)
	{
		//set location to the target
		SetActorLocation(TargetLocation);

		//mark this projectile for destruction
		DespawnTime = 0.0f;
	}
	else
	{

		//travel toward the target
		SetActorLocation(PreviousLocation + TravelVector);
		PreviousLocation = GetActorLocation();

		//Get 'right' direction vector
		FVector CameraLocation;
		FRotator CameraRotation;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLocation,CameraRotation);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *CameraRotation.ToString())
		CameraRotation.Add(0, 90, 0);
		
		//adds curve
		FVector RightCameraVector = CameraRotation.Vector();
		RightCameraVector.Normalize();
		RightCameraVector *= GetCurveAdditive(TravelCompletion);
		SetActorLocation(GetActorLocation()+RightCameraVector);
	}

	
#if 0
	FVector Forward(DeltaTime, 0.0f, 0.0f);
	AddActorLocalOffset(Forward * MoveSpeed);
#endif

}

void AProjectileBase::SetTarget(FVector InTargetLocation)
{
	//set the target
	TargetLocation = InTargetLocation;

	PreviousLocation = GetActorLocation();
	
	//set the initial distance from target
	FVector CurrentToTarget = TargetLocation - GetActorLocation();
	InitDistance = CurrentToTarget.Length();
	
}

float AProjectileBase::GetCurveAdditive(float input)
{
	//parabola
	return ((((input * 2) - 1) * ((input * 2) - 1) * -1) + 1) / (1 / Curviness);
}


