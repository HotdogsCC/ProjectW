// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "WizardCharacter.h"
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
	
	bTargetReached = false;
	bReady = false;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	bTargetReached = false;
	bReady = false;
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		bReady = true;
		
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

		//its current percentage of travel, from 0-1 (or 1-inf past target)
		float TravelCompletion = (InitDistance - CurDistanceToTarget) / InitDistance;
		
		FVector TravelVector = TargetDirection * MoveSpeed * DeltaTime;

		//travel toward the target
		SetActorLocation(PreviousLocation + TravelVector);
		PreviousLocation = GetActorLocation();
		
		//adds curve
		FVector TempCurveVector = CurveDirection;
		
		
		if(!bTargetReached)
		{
			//are we going to overshoot?
			if(TravelVector.SquaredLength() > CurDistanceToTarget * CurDistanceToTarget)
			{
				//set location to the target
				SetActorLocation(TargetLocation);

				//mark this projectile for destruction
				//DespawnTime = 0.0f;

				bTargetReached = true;
				
			}
			else
			{
				TempCurveVector *= GetCurveAdditive(TravelCompletion);
			}
		}
		else
		{
			//weird math bullshit but it works, trust
			TempCurveVector *= GetCurveAdditive(2-TravelCompletion);
		}

		SetActorLocation(GetActorLocation()+TempCurveVector);
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

	//set target direction
	TargetDirection = CurrentToTarget.GetSafeNormal();

	//set initial curve direction
	FRotator DirectionRotator = CurrentToTarget.Rotation();
	DirectionRotator.Add(0, FMath::FRandRange(0.0f, 360.0f), 0);
	CurveDirection = DirectionRotator.Vector();
	CurveDirection.Normalize();
	
	bTargetReached = false;
	
}

float AProjectileBase::GetCurveAdditive(float input)
{
	//parabola
	return ((((input * 2) - 1) * ((input * 2) - 1) * -1) + 1) / (1 / Curviness);
}

void AProjectileBase::SetWizardOwner(AWizardCharacter* Wizard)
{
	WizardOwner = Wizard;
}

AWizardCharacter* AProjectileBase::GetWizardOwner() const
{
	return WizardOwner;
}

bool AProjectileBase::GetReady() const
{
	return bReady;
}

int32 AProjectileBase::GetDamage() const
{
	return Damage;
}



