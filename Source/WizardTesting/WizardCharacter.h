// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "ProjectileBase.h"
#include "WizardCharacter.generated.h"


class UHUDUserWidget;
class AProjectileBase;

USTRUCT()
struct FSpell
{
	GENERATED_BODY()

	EFireType FireType;
	float TimeBetweenShots;
	float BurstModeTime;
};

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

	UFUNCTION(BlueprintCallable)
	void TakeDamage(int32 DamageTaken);

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
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

	UFUNCTION()
	void OnSecondaryFire();
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SecondaryFireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUD_Widget;

	UPROPERTY()
	UHUDUserWidget* HUD_WidgetInstance;
	

	//RPC Primary Fire
	UFUNCTION(Server, Reliable)
	void PrimaryFireServerRPC();
	
	//RPC Secondary Fire
	UFUNCTION(Server, Reliable)
	void SecondaryFireServerRPC();

	//RPC Sprint
	UFUNCTION(Server, Reliable)
	void UpdateSprintRPC(float NewSpeed);

	//Projectile Blueprints
	UPROPERTY(EditDefaultsOnly, Category = "Projectiles", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileBP;

	UPROPERTY(EditDefaultsOnly, Category = "Projectiles", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> SecondaryProjectileBP;

	//Sprint and walk speeds
	UPROPERTY(EditDefaultsOnly, Category = "Player Attributes", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Player Attributes", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

	//temp object used for raycast debugging
	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> SpotterThing;

	//How much health the wizard starts with
	UPROPERTY(EditDefaultsOnly, Category = "Player Attributes", meta = (AllowPrivateAccess = "true"))
	int32 MaxHealth;
	
	//How much health they currently have
	UPROPERTY(Replicated)
	int32 CurrentHealth;

	//what the health was last frame
	UPROPERTY()
	int32 LastKnownHealth;

	//temp for instant respawn
	UPROPERTY()
	FVector SpawnLocation;

	//information about the primary spell type
	UPROPERTY()
	FSpell PrimarySpell;

	//information about the secondary spell type
	UPROPERTY()
	FSpell SecondarySpell;

	UPROPERTY(Replicated)
	bool bCanFire = true;

	UPROPERTY(Replicated)
	bool bCanSecondaryFire = true;

	//networking bullshit
	UPROPERTY()
	bool bLastKnownCanFire = true;
	UPROPERTY()
	bool bLastKnownCanSecondaryFire = true;

	UPROPERTY()
	float CanFireTimer;

	UPROPERTY()
	float CanSecondaryFireTimer;

	UPROPERTY()
	bool bIsBursting = false;
	
	UPROPERTY()
	float CanBurstTimer;

	UPROPERTY()
	uint8 BurstCount;

	UFUNCTION()
	void SpawnProjectile(UClass* ProjectileToSpawn);

};
