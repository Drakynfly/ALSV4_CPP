// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSBaseCharacter.h"
#include "ALSPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FViewModeChangedSignature,
											 class ACharacter*, Character,
											 EALSViewMode, PrevViewMode);

class UALSPlayerCameraBehavior;

/**
 *
 */
UCLASS()
class ALSV4_CPP_API AALSPlayerCharacter : public AALSBaseCharacter
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void OnMovementStateChanged(EALSMovementState PreviousState) override;
	virtual void OnMovementActionChanged(EALSMovementAction PreviousAction) override;
	virtual void OnStanceChanged(EALSStance PreviousStance) override;
	virtual void OnGaitChanged(EALSGait PreviousGait) override;
	virtual void OnRotationModeChanged(EALSRotationMode PreviousRotationMode) override;

	virtual void OnVisibleMeshChanged(const USkeletalMesh* PreviousSkeletalMesh) override;

	virtual bool LimitGroundedRotation() const override;

	/** Camera System */

	UFUNCTION(BlueprintGetter, Category = "ALS|Camera System")
	bool IsRightShoulder() const { return bRightShoulder; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void SetRightShoulder(bool bNewRightShoulder);

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius);

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual FTransform GetThirdPersonPivotTarget();

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual FVector GetFirstPersonCameraTarget();

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void SetCameraBehavior(UALSPlayerCameraBehavior* CamBeh) { CameraBehavior = CamBeh; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const;

protected:
	/** Input */

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void MovementInput_X(float Value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void MovementInput_Y(float Value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void MovementInput_Z(float Value);

	void PlayerCameraUpInput(float Value);

	void PlayerCameraRightInput(float Value);

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
	void Input_Gait();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_Ragdoll();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_VelocityDirection();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input")
	void Input_LookingDirection();

	/** View Mode state */
public:

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetViewMode(EALSViewMode NewViewMode);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetViewMode(EALSViewMode NewViewMode);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSViewMode GetViewMode() const { return ViewMode; }

protected:

	virtual void OnViewModeChanged(EALSViewMode PreviousViewMode);

	/** Multicast delegate for ViewMode changing. */
	UPROPERTY(BlueprintAssignable, Category=Character)
	FViewModeChangedSignature ViewModeChangedDelegate;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnViewModeChanged", ScriptName="OnViewModeChanged"))
	void K2_OnViewModeChanged(EALSViewMode PreviousViewMode);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_ViewMode(EALSViewMode PrevViewMode);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_ViewMode)
	EALSViewMode ViewMode = EALSViewMode::ThirdPerson;

	/** Input */

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float RollDoubleTapTimeout = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float ViewModeSwitchHoldTime = 0.2f;

	/** Camera System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float ThirdPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float FirstPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	bool bRightShoulder = false;

	/** Cached Variables */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Camera")
	UALSPlayerCameraBehavior* CameraBehavior;

	/** Last time the 'first' crouch/roll button is pressed */
	float LastStanceInputTime = 0.0f;

	/** Last time the camera action button is pressed */
	float CameraActionPressedTime = 0.0f;

	/* Timer to manage camera mode swap action */
	FTimerHandle OnCameraModeSwapTimer;
};