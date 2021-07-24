// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ALSPlayerCharacter.h"
#include "Character/ALSCharacterMovementComponent.h"

#include "ALS_Settings.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Kismet/KismetMathLibrary.h"
#include "Library/ALSMathLibrary.h"
#include "Net/UnrealNetwork.h"

const FName NAME_FP_Camera(TEXT("FP_Camera"));

struct FALSAnimCharacterInformation;

void AALSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AALSPlayerCharacter, ViewMode, COND_SkipOwner);
}

void AALSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FALSAnimCharacterInformation& AnimData = MainAnimInstance->GetCharacterInformationMutable();
	AnimData.ViewMode = ViewMode;
	SetViewMode(ViewMode);
}

void AALSPlayerCharacter::OnMovementStateChanged(const EALSMovementState PreviousState)
{
	Super::OnMovementStateChanged(PreviousState);

	if (CameraBehavior)
	{
		CameraBehavior->MovementState = MovementState;
	}
}

void AALSPlayerCharacter::OnMovementActionChanged(const EALSMovementAction PreviousAction)
{
	Super::OnMovementActionChanged(PreviousAction);

	if (CameraBehavior)
	{
		CameraBehavior->MovementAction = MovementAction;
	}
}

void AALSPlayerCharacter::OnStanceChanged(const EALSStance PreviousStance)
{
	Super::OnStanceChanged(PreviousStance);

	if (CameraBehavior)
	{
		CameraBehavior->Stance = Stance;
	}
}

void AALSPlayerCharacter::OnGaitChanged(const EALSGait PreviousGait)
{
	Super::OnGaitChanged(PreviousGait);

	if (CameraBehavior)
	{
		CameraBehavior->Gait = Gait;
	}
}

void AALSPlayerCharacter::OnRotationModeChanged(const EALSRotationMode PreviousRotationMode)
{
	Super::OnRotationModeChanged(PreviousRotationMode);

	if (RotationMode == EALSRotationMode::VelocityDirection && ViewMode == EALSViewMode::FirstPerson)
	{
		// If the new rotation mode is Velocity Direction and the character is in First Person,
		// set the viewmode to Third Person.
		SetViewMode(EALSViewMode::ThirdPerson);
	}

	if (CameraBehavior)
	{
		CameraBehavior->SetRotationMode(RotationMode);
	}
}

void AALSPlayerCharacter::OnVisibleMeshChanged(const USkeletalMesh* PreviousSkeletalMesh)
{
	Super::OnVisibleMeshChanged(PreviousSkeletalMesh);
	FALSAnimCharacterInformation& AnimData = MainAnimInstance->GetCharacterInformationMutable();
	AnimData.ViewMode = ViewMode;
}

bool AALSPlayerCharacter::LimitGroundedRotation() const
{
	return (ViewMode == EALSViewMode::ThirdPerson && RotationMode == EALSRotationMode::Aiming)
			|| ViewMode == EALSViewMode::FirstPerson;
}

FVector AALSPlayerCharacter::GetFirstPersonCameraTarget()
{
	return GetMesh()->GetSocketLocation(NAME_FP_Camera);
}

void AALSPlayerCharacter::SetViewMode(const EALSViewMode NewViewMode)
{
	if (ViewMode != NewViewMode)
	{
		const EALSViewMode Prev = ViewMode;
		ViewMode = NewViewMode;
		OnViewModeChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetViewMode(NewViewMode);
		}
	}
}

ECollisionChannel AALSPlayerCharacter::GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius)
{
	TraceOrigin = GetActorLocation();
	TraceRadius = 10.0f;
	return ECC_Visibility;
}

FTransform AALSPlayerCharacter::GetThirdPersonPivotTarget()
{
	return GetActorTransform();
}

void AALSPlayerCharacter::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
	if (CameraBehavior)
	{
		CameraBehavior->bRightShoulder = bRightShoulder;
	}
}

void AALSPlayerCharacter::GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const
{
	TPFOVOut = ThirdPersonFOV;
	FPFOVOut = FirstPersonFOV;
	bRightShoulderOut = bRightShoulder;
}

void AALSPlayerCharacter::MovementInput_X(const float Value)
{
	InputVector.X = Value;

	if (Value == 0.f) return;

	// Default camera relative movement behavior
	const float Scale = UALSMathLibrary::FixDiagonalGamepadValues(Value, InputVector.Y).Key;

	float Pitch = AimingRotation.Pitch;
	if (GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		const float AltitudeAdjustedAngle = MaxFlightForwardAngle * AtmosphereAtAltitude;

		if (Value >= 0.f) // Going forward
		{
			Pitch = FMath::ClampAngle(AimingRotation.Pitch, -MaxFlightForwardAngle,
			                          AltitudeAdjustedAngle);
		}
		else // Going backward
		{
			Pitch = FMath::ClampAngle(AimingRotation.Pitch, -AltitudeAdjustedAngle,
			                          MaxFlightForwardAngle);
		}
	}
	const FRotator DirRotator(Pitch, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Scale);

	//if (MovementState == EALSMovementState::Grounded || MovementState == EALSMovementState::InAir)
	//{
	//	// Default camera relative movement behavior
	//	const float Scale = UALSMathLibrary::FixDiagonalGamepadValues(GetInputAxisValue("MoveForward/Backwards"), Value)
	//		.Value;
	//	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	//	AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Scale);
	//}
}

void AALSPlayerCharacter::MovementInput_Y(const float Value)
{
	InputVector.Y = Value;

	if (Value == 0.f) return;

	// Default camera relative movement behavior
	const float Scale = UALSMathLibrary::FixDiagonalGamepadValues(InputVector.X, Value).Value;
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Scale);


	//if (MovementState == EALSMovementState::Grounded || MovementState == EALSMovementState::InAir)
	//{
	//	// Default camera relative movement behavior
	//	const float Scale = UALSMathLibrary::FixDiagonalGamepadValues(Value, GetInputAxisValue("MoveRight/Left")).Key;
	//	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	//	AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Scale);
	//}
}

void AALSPlayerCharacter::MovementInput_Z(const float Value)
{
	InputVector.Z = Value;

	if (Value == 0.f) return;

	// Default camera relative movement behavior
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetUpVector(DirRotator), Value);
}

void AALSPlayerCharacter::PlayerCameraUpInput(const float Value)
{
	AddControllerPitchInput(LookUpDownRate * Value);
}

void AALSPlayerCharacter::PlayerCameraRightInput(const float Value)
{
	AddControllerYawInput(LookLeftRightRate * Value);
}

void AALSPlayerCharacter::Input_Jump()
{
	if (JumpPressedDelegate.IsBound())
	{
		JumpPressedDelegate.Broadcast();
	}

	if (MovementAction != EALSMovementAction::None) return; // Guard against jumping while doing something else.

	if (MovementState == EALSMovementState::Grounded)
	{
		if (Stance == EALSStance::Standing) { Jump(); }
		else if (Stance == EALSStance::Crouching) { UnCrouch(); }
	}
	else if (MovementState == EALSMovementState::Freefall)
	{
		//SetFlightMode(EALSFlightMode::Neutral);
	}
	else if (MovementState == EALSMovementState::Flight)
	{
		//if (FlightMode == EALSFlightMode::Hovering)
		//{
		//	SetFlightMode(EALSFlightMode::Aerial);
		//}
	}
	else if (MovementState == EALSMovementState::Ragdoll)
	{
		ReplicatedRagdollEnd();
	}
}

void AALSPlayerCharacter::Input_Jump_Release() { StopJumping(); }

void AALSPlayerCharacter::Input_Sprint() { SetDesiredGait(EALSGait::Sprinting); }

void AALSPlayerCharacter::Input_Sprint_Release() { SetDesiredGait(EALSGait::Running); }

void AALSPlayerCharacter::Input_Aim()
{
	// AimAction: Hold "AimAction" to enter the aiming mode, release to revert back the desired rotation mode.
	SetRotationMode(EALSRotationMode::Aiming);
}

void AALSPlayerCharacter::Input_Aim_Release()
{
	if (ViewMode == EALSViewMode::ThirdPerson) { SetRotationMode(DesiredRotationMode); }
	else if (ViewMode == EALSViewMode::FirstPerson) { SetRotationMode(EALSRotationMode::LookingDirection); }
}

void AALSPlayerCharacter::Input_Camera_Action()
{
	UWorld* World = GetWorld();
	check(World);
	CameraActionPressedTime = World->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(OnCameraModeSwapTimer,
									this,
									&AALSPlayerCharacter::OnSwitchCameraMode,
									ViewModeSwitchHoldTime,
									false);
}

void AALSPlayerCharacter::Input_Camera_Action_Release()
{
	if (ViewMode == EALSViewMode::FirstPerson) { return; } // Don't swap shoulders on first person mode

	UWorld* World = GetWorld();
	check(World);
	if (World->GetTimeSeconds() - CameraActionPressedTime < ViewModeSwitchHoldTime)
	{
		// Switch shoulders
		SetRightShoulder(!bRightShoulder);
		GetWorldTimerManager().ClearTimer(OnCameraModeSwapTimer); // Prevent mode change
	}
}

void AALSPlayerCharacter::OnSwitchCameraMode()
{
	// Switch camera mode
	if (ViewMode == EALSViewMode::FirstPerson) { SetViewMode(EALSViewMode::ThirdPerson); }
	else if (ViewMode == EALSViewMode::ThirdPerson) { SetViewMode(EALSViewMode::FirstPerson); }
}

void AALSPlayerCharacter::Input_Stance()
{
	// Stance Action: Press "Stance Action" to toggle Standing / Crouching, double tap to Roll.

	if (MovementAction != EALSMovementAction::None) return;

	UWorld* World = GetWorld();
	check(World);

	const float PrevStanceInputTime = LastStanceInputTime;
	LastStanceInputTime = World->GetTimeSeconds();

	if (LastStanceInputTime - PrevStanceInputTime <= RollDoubleTapTimeout)
	{
		// Roll
		Replicated_PlayMontage(GetRollAnimation(), 1.15f);

		if (Stance == EALSStance::Standing) { SetDesiredStance(EALSStance::Crouching); }
		else if (Stance == EALSStance::Crouching) { SetDesiredStance(EALSStance::Standing); }
		return;
	}

	if (MovementState == EALSMovementState::Grounded)
	{
		if (Stance == EALSStance::Standing)
		{
			SetDesiredStance(EALSStance::Crouching);
			Crouch();
		}
		else if (Stance == EALSStance::Crouching)
		{
			SetDesiredStance(EALSStance::Standing);
			UnCrouch();
		}
	}
}

void AALSPlayerCharacter::Input_Gait()
{
	if (DesiredGait == EALSGait::Walking)
	{
		SetDesiredGait(EALSGait::Running);
	}
	else if (DesiredGait == EALSGait::Running)
	{
		SetDesiredGait(EALSGait::Walking);
	}
}

void AALSPlayerCharacter::Input_Ragdoll()
{
	// Ragdoll Action: Press "Ragdoll Action" to toggle the ragdoll state on or off.

	if (GetMovementState() == EALSMovementState::Ragdoll)
	{
		ReplicatedRagdollEnd();
	}
	else
	{
		ReplicatedRagdollStart();
	}
}

void AALSPlayerCharacter::Input_VelocityDirection()
{
	// Select Rotation Mode: Switch the desired (default) rotation mode to Velocity or Looking Direction.
	// This will be the mode the character reverts back to when un-aiming
	SetDesiredRotationMode(EALSRotationMode::VelocityDirection);
	SetRotationMode(EALSRotationMode::VelocityDirection);
}

void AALSPlayerCharacter::Input_LookingDirection()
{
	SetDesiredRotationMode(EALSRotationMode::LookingDirection);
	SetRotationMode(EALSRotationMode::LookingDirection);
}

void AALSPlayerCharacter::Server_SetViewMode_Implementation(const EALSViewMode NewViewMode)
{
	SetViewMode(NewViewMode);
}

void AALSPlayerCharacter::OnViewModeChanged(const EALSViewMode PreviousViewMode)
{
	MainAnimInstance->GetCharacterInformationMutable().ViewMode = ViewMode;
	switch (ViewMode)
	{
	case EALSViewMode::ThirdPerson:
		if (RotationMode == EALSRotationMode::VelocityDirection || RotationMode == EALSRotationMode::LookingDirection)
		{
			// If Third Person, set the rotation mode back to the desired mode.
			SetRotationMode(DesiredRotationMode);
		}
		break;
	case EALSViewMode::FirstPerson:
		{
			if (RotationMode == EALSRotationMode::VelocityDirection)
			{
				// If First Person, set the rotation mode to looking direction if currently in the velocity direction mode.
				SetRotationMode(EALSRotationMode::LookingDirection);
			}
		}
		break;
	default: ;
	}

	K2_OnViewModeChanged(PreviousViewMode);
	ViewModeChangedDelegate.Broadcast(this, PreviousViewMode);

	if (CameraBehavior)
	{
		CameraBehavior->ViewMode = ViewMode;
	}
}

void AALSPlayerCharacter::OnRep_ViewMode(const EALSViewMode PrevViewMode)
{
	OnViewModeChanged(PrevViewMode);
}