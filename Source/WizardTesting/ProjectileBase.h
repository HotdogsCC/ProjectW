// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"


class AWizardCharacter;
class UPointLightComponent;

UENUM()
enum class EFireType : uint8
{
	Single,
	Burst,
	Automatic
};


UCLASS()
class WIZARDTESTING_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// used for where the spell should travel to 
	void SetTarget(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	AWizardCharacter* GetWizardOwner() const;
		
	void SetWizardOwner(AWizardCharacter* Wizard);

	UFUNCTION(BlueprintCallable)
	bool GetReady() const;

	UFUNCTION(BlueprintCallable)
	int32 GetDamage() const;

	UFUNCTION()
	EFireType GetFireType() const;

	UFUNCTION()
	float GetTimeBetweenShots() const;

	UFUNCTION()
	float GetBurstModeTime() const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* LightComponent;

	//how long the projectile is alive for
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float DespawnTime = 3.0f;

	//how fast the projectile is
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float MoveSpeed = 5000.0f;

	//the curviness of the projectile
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float Curviness = 0.0f;

	//how much damage the projectile deals
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int32 Damage = 10;

	//the initial distance from its spawn to the target
	UPROPERTY()
	float InitDistance;
	
	//where the projectile should head toward
	UPROPERTY()
	FVector TargetLocation;

	//the direction in which the projectile should travel
	UPROPERTY()
	FVector TargetDirection;

	UFUNCTION()
	float GetCurveAdditive(float input);

	//where the orb just was
	UPROPERTY()
	FVector PreviousLocation;

	UPROPERTY()
	FVector CurveDirection;

	UPROPERTY()
	bool bTargetReached;

	//the character actor who spawned the projectile
	UPROPERTY()
	AWizardCharacter* WizardOwner;

	//whether the projectile should care about collisions
	UPROPERTY()
	bool bReady;

	//the kind of fire mode of the projectile
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	EFireType FireType;

	//time between each projectile to spawn
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float TimeBetweenShots = 1.0f;

	//time between each projectile to spawn for bursts
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float BurstModeTime = 0.1f;
};
