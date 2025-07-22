// Fill out your copyright notice in the Description page of Project Settings.


#include "WizardCharacter.h"
#include "ProjectileBase.h"

// Sets default values
AWizardCharacter::AWizardCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWizardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			ECC_Visibility,
			QueryParams
			);

		//if we hit something
		if(bHit)
		{

			AActor* ProjectileInstance = GetWorld()->SpawnActor(ProjectileBP);
			//spawns projectile where player is
			ProjectileInstance->SetActorLocation(GetActorLocation());
			//rotates projectile in the direction the player is looking
			ProjectileInstance->SetActorRotation(GetControlRotation());
			//get projectile base cpp
			if(AProjectileBase* ProjectileComponent = Cast<AProjectileBase>(ProjectileInstance))
			{
				ProjectileComponent->SetTarget(HitResult.Location);
			}
		}
		

		
	}
}




