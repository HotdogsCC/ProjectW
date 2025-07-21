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
	//FVector
	
	FVector Forward(DeltaTime, 0.0f, 0.0f);
	AddActorLocalOffset(Forward * MoveSpeed);

}

