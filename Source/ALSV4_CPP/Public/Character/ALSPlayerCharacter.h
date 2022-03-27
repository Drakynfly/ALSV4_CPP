// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ALSCharacter.h"
#include "ALSPlayerCharacter.generated.h"

class UALSPlayerCameraBehavior;

/**
 *
 */
UCLASS()
class ALSV4_CPP_API AALSPlayerCharacter : public AALSCharacter
{
	GENERATED_BODY()

protected:
	/** Input */

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void MovementInput_X(float Value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void MovementInput_Y(float Value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void MovementInput_Z(float Value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Jump();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Jump_Release();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Sprint();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Sprint_Release();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Aim();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Aim_Release();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Camera_Action();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Camera_Action_Release();

	void OnSwitchCameraMode();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Stance();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Roll();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Gait();

	/** Input */

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float RollAnimationSpeed = 1.15f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float ViewModeSwitchHoldTime = 0.2f;


	/** Cached Variables */


	/** Last time the camera action button is pressed */
	float CameraActionPressedTime = 0.0f;

	/** Timer to prevent roll input spamming */
	float RollResetTimer = 0.0f;

	/** Timer to manage camera mode swap action */
	FTimerHandle OnCameraModeSwapTimer;
};