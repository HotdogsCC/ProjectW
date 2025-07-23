// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "WizardCharacter.generated.h"


class AProjectileBase;

UCLASS()
class WIZARDTESTING_API AWizardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWizardCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	//Input Functions
	UFUNCTION()
	void OnMove(const FInputActionValue& Value);

	UFUNCTION()
	void OnLook(const FInputActionValue& Value);

	UFUNCTION()
	void OnStartSprint();

	UFUNCTION()
	void OnStopSprint();

	UFUNCTION()
	void OnPrimaryFire();
	
	//Input actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryFireAction;

	//RPC Primary Fire
	UFUNCTION(Server, Reliable)
	void PrimaryFireServerRPC();

	//Projectile Blueprints
	UPROPERTY(EditDefaultsOnly, Category = "Projectiles", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileBP;

	//Sprint and walk speeds
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

	//temp object used for raycast debugging
	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> SpotterThing;

};
