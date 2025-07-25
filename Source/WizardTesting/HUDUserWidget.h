// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class WIZARDTESTING_API UHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealthUI(int32 MaxHealth, int32 CurrentHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void CannotFire();

	UFUNCTION(BlueprintImplementableEvent)
	void CanFire();
};
