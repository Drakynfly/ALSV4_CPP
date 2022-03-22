// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Character/ALSPlayerCharacter.h"
#include "Character/ALSCharacterMovementComponent.h"

#include "ALS_Settings.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Kismet/KismetMathLibrary.h"

struct FALSAnimCharacterInformation;

void AALSPlayerCharacter::MovementInput_X(const float Value)
{
	if (Value == 0.f) return;

	// Default camera relative movement behavior

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
	AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);

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
	if (Value == 0.f) return;

	// Default camera relative movement behavior
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);


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
	if (Value == 0.f) return;

	// Default camera relative movement behavior
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetUpVector(DirRotator), Value);
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
		SetFlightState(EALSFlightState::Aerial);
	}
	else if (MovementState == EALSMovementState::Flight)
	{
		if (FlightState == EALSFlightState::Hovering)
		{
			SetFlightState(EALSFlightState::Aerial);
		}
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
	if (MovementAction != EALSMovementAction::None) return;

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

void AALSPlayerCharacter::Input_Roll()
{
	UAnimMontage* Animation = GetRollAnimation();
	const float AnimPlayLength = Animation->GetPlayLength() / RollAnimationSpeed;

	const float ThisRollTime = GetWorld()->GetTimeSeconds();

	if (ThisRollTime - RollResetTimer >= AnimPlayLength)
	{
		Replicated_PlayMontage(Animation, RollAnimationSpeed);
		RollResetTimer = ThisRollTime;
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