// Fill out your copyright notice in the Description page of Project Settings.


#include "WizardCharacter.h"
#include "Net/UnrealNetwork.h"
#include "HUDUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AWizardCharacter::AWizardCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//set max walk speed to walk speed definied in wizard BP
	if(UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement())
	{
		MyCharacterMovement->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to set character movement in constructor"));
	}

}

// Called when the game starts or when spawned
void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = GetActorLocation();

	AActor* ProjectileActorInstance = GetWorld()->SpawnActor(ProjectileBP);
	if(ProjectileActorInstance)
	{
		AProjectileBase* TempProjectileBase = Cast<AProjectileBase>(ProjectileActorInstance);
		if(TempProjectileBase)
		{
			PrimarySpell.FireType = TempProjectileBase->GetFireType();
			PrimarySpell.TimeBetweenShots = TempProjectileBase->GetTimeBetweenShots();
			PrimarySpell.BurstModeTime = TempProjectileBase->GetBurstModeTime();
		}
		ProjectileActorInstance->Destroy();
	}

	AActor* Projectile2ActorInstance = GetWorld()->SpawnActor(SecondaryProjectileBP);
	if(Projectile2ActorInstance)
	{
		AProjectileBase* TempProjectileBase = Cast<AProjectileBase>(Projectile2ActorInstance);
		if(TempProjectileBase)
		{
			SecondarySpell.FireType = TempProjectileBase->GetFireType();
			SecondarySpell.TimeBetweenShots = TempProjectileBase->GetTimeBetweenShots();
			SecondarySpell.BurstModeTime = TempProjectileBase->GetBurstModeTime();
		}
		Projectile2ActorInstance->Destroy();
	}
	
	

	//set max walk speed to walk speed definied in wizard BP
	if(UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement())
	{
		MyCharacterMovement->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to set character movement in constructor"));
	}

	CurrentHealth = MaxHealth;
	LastKnownHealth = CurrentHealth;

	if(HUD_Widget)
	{
		UUserWidget* TempWidget = CreateWidget(Cast<APlayerController>(GetController()), HUD_Widget);
		HUD_WidgetInstance = Cast<UHUDUserWidget>(TempWidget);
		if(HUD_WidgetInstance)
		{
			HUD_WidgetInstance->AddToViewport();

			HUD_WidgetInstance->CanFire();
		}
	}
	
}

// Called every frame
void AWizardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//has our health changed? (networking bullshit)
	if(LastKnownHealth != CurrentHealth)
	{
		LastKnownHealth = CurrentHealth;
		//update the UI
		if(HUD_WidgetInstance)
		{
			HUD_WidgetInstance->UpdateHealthUI(MaxHealth, CurrentHealth);
		}
	}

	//has our firing status changed?
	if(bLastKnownCanFire != bCanFire)
	{
		bLastKnownCanFire = bCanFire;
		if(HUD_WidgetInstance)
		{
			if(bCanFire)
			{
				HUD_WidgetInstance->CanFire();
			}
			else
			{
				HUD_WidgetInstance->CannotFire();
			}
		}
	}

	//has our firing status changed?
	if(bLastKnownCanSecondaryFire != bCanSecondaryFire)
	{
		bLastKnownCanSecondaryFire = bCanSecondaryFire;
		
	}

	//are we the server?
	if(HasAuthority())
	{
		//are we on a firing cooldown?
		if(!bCanFire)
		{
			CanFireTimer -= DeltaTime;
			if(CanFireTimer <= 0.0f)
			{
				bCanFire = true;
			}
		}

		//are we on a firing cooldown?
		if(!bCanSecondaryFire)
		{
			CanSecondaryFireTimer -= DeltaTime;
			if(CanSecondaryFireTimer <= 0.0f)
			{
				bCanSecondaryFire = true;
			}
		}

		//are we currently bursting?
		if(bIsBursting)
		{
			CanBurstTimer -= DeltaTime;

			if(CanBurstTimer <= 0.0f)
			{
				SpawnProjectile(ProjectileBP);
				BurstCount++;
				if(BurstCount > 1)
				{
					bIsBursting = false;
				}
				else
				{
					CanBurstTimer = PrimarySpell.BurstModeTime;
				}
			}
		}
	}
	
	
}

// Called to bind functionality to input
void AWizardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up action bindings
	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWizardCharacter::OnMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWizardCharacter::OnLook);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(PrimaryFireAction, ETriggerEvent::Started, this, &AWizardCharacter::OnPrimaryFire);
		EnhancedInputComponent->BindAction(SecondaryFireAction, ETriggerEvent::Started, this, &AWizardCharacter::OnSecondaryFire);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AWizardCharacter::OnStartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AWizardCharacter::OnStopSprint);
	}

}

void AWizardCharacter::OnMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWizardCharacter::OnLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWizardCharacter::OnPrimaryFire()
{
	PrimaryFireServerRPC();
}

void AWizardCharacter::PrimaryFireServerRPC_Implementation()
{
	//if this machine is the server
	if(HasAuthority())
	{
		//can we shoot?
		if(bCanFire)
		{
			bCanFire = false;
			CanFireTimer = PrimarySpell.TimeBetweenShots;
			
			switch (PrimarySpell.FireType)
			{
			case EFireType::Single:
				break;

			case EFireType::Burst:
				bIsBursting = true;
				CanBurstTimer = PrimarySpell.BurstModeTime;
				BurstCount = 0;
				break;

			case EFireType::Automatic:
				break;
			}
			
			SpawnProjectile(ProjectileBP);
			
		}
		
	}
}

void AWizardCharacter::OnSecondaryFire()
{
	SecondaryFireServerRPC();
}

void AWizardCharacter::SecondaryFireServerRPC_Implementation() 
{
	//if this machine is the server
	if(HasAuthority())
	{
		//can we shoot?
		if(bCanSecondaryFire)
		{
			bCanSecondaryFire = false;
			CanSecondaryFireTimer = SecondarySpell.TimeBetweenShots;
			
			switch (SecondarySpell.FireType)
			{
			case EFireType::Single:
				break;

			case EFireType::Burst:
				
				break;

			case EFireType::Automatic:
				break;
			}
			
			SpawnProjectile(SecondaryProjectileBP);
			
		}
		
	}
}


void AWizardCharacter::OnStartSprint()
{
	//update server
	UpdateSprintRPC(SprintSpeed);

	//update client
	if(UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement())
	{
		MyCharacterMovement->MaxWalkSpeed = SprintSpeed;
	}
}

void AWizardCharacter::OnStopSprint()
{
	UpdateSprintRPC(WalkSpeed);

	//update client
	if(UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement())
	{
		MyCharacterMovement->MaxWalkSpeed = WalkSpeed;
	}
	
}

void AWizardCharacter::TakeDamage(int32 DamageTaken)
{
	//remove the taken damage from health
	CurrentHealth -= DamageTaken;

	//check if the wizard should die
	if(CurrentHealth <= 0)
	{
		//die
		UE_LOG(LogTemp, Warning, TEXT("buddy is out of health and should die"));
		SetActorLocation(SpawnLocation);
		CurrentHealth = MaxHealth;
	}
	
}


void AWizardCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWizardCharacter, CurrentHealth);
	DOREPLIFETIME(AWizardCharacter, bCanFire);
	DOREPLIFETIME(AWizardCharacter, bCanSecondaryFire);
}

void AWizardCharacter::UpdateSprintRPC_Implementation(float NewSpeed)
{
	if(HasAuthority())
	{
		if(UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement())
		{
			MyCharacterMovement->MaxWalkSpeed = NewSpeed;
		}
	}
}

void AWizardCharacter::SpawnProjectile(UClass* ProjectileToSpawn)
{
	//get camera transform
	FVector CameraLocation;
	FRotator CameraRotation;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector End = CameraLocation + (CameraRotation.Vector() * 20000.0f);

	//set up for trace
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	//do the trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		End,
		ECC_WorldStatic,
		QueryParams
		);

	//if we hit something
	if(bHit)
	{
		AActor* ProjectileInstance = GetWorld()->SpawnActor(ProjectileToSpawn);
		//spawns projectile where player is
		ProjectileInstance->SetActorLocation(GetActorLocation());
		//rotates projectile in the direction the player is looking
		ProjectileInstance->SetActorRotation(GetControlRotation());
		//get projectile base cpp
		if(AProjectileBase* ProjectileComponent = Cast<AProjectileBase>(ProjectileInstance))
		{
			ProjectileComponent->SetWizardOwner(this);
			ProjectileComponent->SetTarget(HitResult.Location);
				
		}
	}
}
