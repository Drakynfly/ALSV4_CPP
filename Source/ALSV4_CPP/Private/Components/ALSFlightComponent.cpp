// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.


#include "Components/ALSFlightComponent.h"

#include "ALS_Settings.h"
#include "Character/ALSCharacterMovementComponent.h"
#include "Components/ALSDebugComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

static bool ALSDebugFlightTraces = false;
FAutoConsoleVariableRef CVarALSDebugFlight(TEXT("ALS.Debug.FlightTraces"), ALSDebugFlightTraces, TEXT("Show debug flight traces"));

UALSFlightComponent::UALSFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	const UALS_Settings* UALS_Settings = UALS_Settings::Get();
	SeaAltitude = UALS_Settings->SeaAltitude;
	TroposphereHeight = UALS_Settings->TroposphereHeight;
}

void UALSFlightComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		OwnerCharacter = Cast<AALSBaseCharacter>(GetOwner());
		if (IsValid(OwnerCharacter))
		{
			ALSDebugComponent = OwnerCharacter->FindComponentByClass<UALSDebugComponent>();

			// @TODO flight catching should really be in the movement state machine not in the flight component.
			OwnerCharacter->MovementModeChangedDelegate.AddDynamic(this, &UALSFlightComponent::OnOwningMovementModeChanged);

			OwnerCharacter->OnActorHit.AddDynamic(this, &UALSFlightComponent::OnActorHit);
		}
	}
}

// @TODO flight catching should really be in the movement state machine not in the flight component.

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UALSFlightComponent::OnOwningMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode,
                                                      uint8 PreviousCustomMode)
{
	if (Character->GetCharacterMovement()->IsFalling())
	{
		TimeStartingFalling = FDateTime::Now();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UALSFlightComponent::OnActorHit(AActor* SelfActor, AActor* OtherActor, const FVector NormalImpulse,
                                     const FHitResult& Hit)
{
	if (OwnerCharacter->GetFlightState() != EALSFlightState::None)
	{
		if ((UseFlightInterrupt && FlightInterruptCheck(OtherActor, NormalImpulse, Hit))
			|| RelativeAltitude <= 5.f)
		{
			OwnerCharacter->SetFlightState(EALSFlightState::None);
		}
	}
}

void UALSFlightComponent::UpdateFlight(const float DeltaTime)
{
	RelativeAltitude = FlightDistanceCheck(TroposphereHeight, FVector::DownVector);

	UpdateFlightRotation(DeltaTime);

	if (OwnerCharacter->HasAuthority() || GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UpdateFlightMovement(DeltaTime);
	}
}

void UALSFlightComponent::UpdateFlightRotation(const float DeltaTime)
{
	const FRotator ActorRotation = OwnerCharacter->GetActorRotation();
	const float SpeedCache = OwnerCharacter->GetMyMovementComponent()->GetMappedSpeed();
	const float CheckAltitude = SpeedCache * 100.f;

	const float FlightRotationRate = CalculateFlightRotationRate();

	// Map distance to ground to a unit scalar.
	const float Alpha_Altitude = FMath::GetMappedRangeValueClamped(FVector2f{0.f, CheckAltitude}, FVector2f{0.f, 1.f}, RelativeAltitude);

	// Combine unit scalars equal to smaller.
	const float RotationAlpha = Alpha_Altitude * (SpeedCache / 3);

	// Calculate input leaning.
	const FVector Lean = ActorRotation.UnrotateVector(OwnerCharacter->GetMovementInput().GetSafeNormal()) * MaxFlightLean * RotationAlpha;

	const float Pitch = FMath::FInterpTo(ActorRotation.Pitch, Lean.X * -1, DeltaTime, FlightRotationRate);
	const float Roll = FMath::FInterpTo(ActorRotation.Roll, Lean.Y, DeltaTime, FlightRotationRate);

	const bool bCanUpdateMovingRot = ((OwnerCharacter->IsMoving() && OwnerCharacter->HasMovementInput())
		|| OwnerCharacter->GetSpeed() > 150.0f) && !OwnerCharacter->HasAnyRootMotion();
	if (bCanUpdateMovingRot)
	{
		FRotator LastVelocityRotation = OwnerCharacter->GetLastVelocityRotation();
		FRotator AimingRotation = OwnerCharacter->GetAimingRotation();

		switch (OwnerCharacter->GetRotationMode())
		{
		case EALSRotationMode::VelocityDirection:
			{
				const float InterpSpeed = FMath::GetMappedRangeValueClamped<float, float>({0.f, 3.f}, {0.1f, FlightRotationRate}, SpeedCache);
				OwnerCharacter->SmoothCharacterRotation({Pitch, LastVelocityRotation.Yaw, Roll}, 100.0f, InterpSpeed, DeltaTime);
				break;
			}
		case EALSRotationMode::LookingDirection:
			{
				float YawValue;
				if (OwnerCharacter->GetGait() == EALSGait::Sprinting)
				{
					YawValue = LastVelocityRotation.Yaw;
				}
				else
				{
					// Walking or Running..
					//const float YawOffsetCurveVal = MainAnimInstance->GetCurveValue(NAME_YawOffset);
					YawValue = AimingRotation.Yaw; //+ YawOffsetCurveVal;
				}
				OwnerCharacter->SmoothCharacterRotation({Pitch, YawValue, Roll}, 100.0f, FlightRotationRate, DeltaTime);
				break;
			}
		case EALSRotationMode::Aiming:
			{
				// Aiming Rotation and looking direction
				const float FinalRoll = Roll + AimingRotation.Roll / 2;
				OwnerCharacter->SmoothCharacterRotation({Pitch, AimingRotation.Yaw, FinalRoll}, 500.0f, FlightRotationRate, DeltaTime);
				break;
			}
		default: ;
		}
	}
	else
	{
		OwnerCharacter->SmoothCharacterRotation({0, ActorRotation.Yaw, 0}, 500.0f, FlightRotationRate, DeltaTime);
	}
}

float UALSFlightComponent::CalculateFlightRotationRate() const
{
	// Calculate the rotation rate by using the current Rotation Rate Curve in the Movement Settings.
	// Using the curve in conjunction with the mapped speed gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	const float MappedSpeedVal = OwnerCharacter->GetMyMovementComponent()->GetMappedSpeed();
	const float CurveVal =
		OwnerCharacter->GetMyMovementComponent()->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped(FVector2f{0.0f, 300.0f}, FVector2f{1.0f, 3.0f}, OwnerCharacter->GetAimYawRate());
	return CurveVal * ClampedAimYawRate;
}

void UALSFlightComponent::UpdateFlightMovement(const float DeltaTime)
{
	if (AlwaysCheckFlightConditions)
	{
		if (!CanFly())
		{
			OwnerCharacter->SetFlightState(EALSFlightState::None);
			return;
		}
	}

	// The rest of this function calculates the auto-hover strength. This is how much downward force is generated by
	// the wings to keep the character afloat.
	float AutoHover;

	// Represents the strength of the wings forcing downward when flying, measured in the units below the character that
	// the pressure gradient extends.
	const float WingPressureDepth = 200; //@todo old calculation: FlightStrengthPassive / EffectiveWeight;

	FVector VelocityDirection;
	float VelocityLength;
	OwnerCharacter->GetVelocity().ToDirectionAndLength(VelocityDirection, VelocityLength);

	const float VelocityAlpha = FMath::GetMappedRangeValueClamped(FVector2f{0, OwnerCharacter->GetCharacterMovement()->MaxFlySpeed * 1.5f},
																  FVector2f{0, 1},
																  VelocityLength);

	const FVector PressureDirection = FMath::Lerp(FVector(0, 0, -1), -VelocityDirection, VelocityAlpha);

	const float PressureAlpha = FlightDistanceCheck(WingPressureDepth, PressureDirection) / WingPressureDepth;

	// If a pressure curve is used, modify speed. Otherwise, default to 1 for no effect.
	float GroundPressure;
	//if (GroundPressureFalloff)
	//{
	//	GroundPressure = GroundPressureFalloff->GetFloatValue(PressureAlpha);
	//}
	//else
	{
		GroundPressure = 1;
	}

	const float LocalTemperatureAffect = 1; // TemperatureAffect.Y;
	const float LocalWeightAffect = 1; //WeightAffect.Y;

	// @TODO Design an algorithm for calculating thrust, and use it to determine lift. modify auto-thrust with that so that the player slowly drifts down when too heavy.

	switch (OwnerCharacter->GetFlightState())
	{
	case EALSFlightState::None: return;
	case EALSFlightState::Hovering:
		AutoHover = (GroundPressure + 0.5) / 1.5 * LocalTemperatureAffect * (LocalWeightAffect / 2);
		break;
	case EALSFlightState::Aerial:
		AutoHover = (GroundPressure + 0.5) / 1.5 * LocalTemperatureAffect * LocalWeightAffect;
		break;
	//case EALSFlightState::Raising:
	//	AutoHover = (GroundPressure + FlightStrengthActive) * LocalTemperatureAffect * (
	//		LocalWeightAffect * 1.5);
	//	break;
	//case EALSFlightState::Lowering:
	//	AutoHover = (GroundPressure * 0.5f) + (-FlightStrengthActive + (LocalTemperatureAffect
	//		- 1)) + -LocalWeightAffect;
	//	break;
	default: return;
	}

	const FRotator DirRotator(0.0f, OwnerCharacter->GetAimingRotation().Yaw, 0.0f);
	//AddMovementInput(UKismetMathLibrary::GetUpVector(DirRotator), AutoHover, true);
}

// @TODO flight catching should really be in the movement state machine not in the flight component.
bool UALSFlightComponent::WantsToCatchFalling_Implementation() const
{
	if (TimeToWaitBeforeCatchFalling >= 0)
	{
		if (FDateTime::Now() - TimeStartingFalling >= FTimespan::FromSeconds(TimeToWaitBeforeCatchFalling))
		{
			return true;
		}
	}

	return false;
}

float UALSFlightComponent::FlightDistanceCheck(float CheckDistance, FVector Direction) const
{
	UWorld* World = GetWorld();
	if (!World) return 0.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	const FVector CheckStart = OwnerCharacter->GetActorLocation() - FVector{0, 0,
		OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
	const FVector CheckEnd = CheckStart + (Direction * CheckDistance);
	bool bHit = World->LineTraceSingleByChannel(HitResult, CheckStart, CheckEnd, UALS_Settings::Get()->FlightCheckChannel, Params);

	if (ALSDebugFlightTraces)
	{
		ALSDebugComponent->DrawDebugLineTraceSingle(World, CheckStart, CheckEnd, EDrawDebugTrace::ForOneFrame,
			bHit, HitResult, FLinearColor::Yellow, FLinearColor::Red, 0.f);
	}

	if (HitResult.bBlockingHit) { return HitResult.Distance; }
	return CheckDistance;
}

void UALSFlightComponent::AdjustFlightInput(FVector& WorldDirection, float& ScaleValue)
{
	if (ScaleValue == 0.f) return;

	/**
	const float AltitudeAdjustedAngle = MaxFlightForwardAngle * AtmosphereAtAltitude;

	float Pitch = OwnerCharacter->GetAimingRotation().Pitch;
	if (ScaleValue >= 0.f) // Going forward
	{
		Pitch = FMath::ClampAngle(Pitch, -MaxFlightForwardAngle,
								  AltitudeAdjustedAngle);
	}
	else // Going backward
	{
		Pitch = FMath::ClampAngle(Pitch, -AltitudeAdjustedAngle,
								  MaxFlightForwardAngle);
	}

	const FRotator DirRotator(Pitch, AimingRotation.Yaw, 0.0f);
	*/

	// Prevent the player from flying above world max height.
	const FVector PlanarDirection = FVector(WorldDirection.X, WorldDirection.Y, 0).GetSafeNormal();
	const FVector NewDirection = FMath::Lerp(PlanarDirection, WorldDirection, AtmosphereAtAltitude);

	if (ALSDebugFlightTraces)
	{
		const FVector Start = OwnerCharacter->GetActorLocation();
		const FVector PlanarDirectionEnd = Start + (PlanarDirection * 100);
		const FVector NewDirectionEnd = Start + (NewDirection * 100);
		const FVector WorldDirectionEnd = Start + (WorldDirection * 100);
		DrawDebugLine(GetWorld(), Start, PlanarDirectionEnd, FColor::Blue, false, 0, 1, 2);
		DrawDebugLine(GetWorld(), Start, NewDirectionEnd, FColor::Cyan, false, 0, 1, 2);
		DrawDebugLine(GetWorld(), Start, WorldDirectionEnd, FColor::Green, false, 0, 1, 2);
	}

	WorldDirection = NewDirection;
}

bool UALSFlightComponent::CanFly() const
{
	return OwnerCharacter->GetMyMovementComponent()->CanEverFly() && FlightCheck();
}

bool UALSFlightComponent::FlightCheck_Implementation() const
{
	return true;
	// @todo
	//&& FMath::IsWithin(Temperature, FlightTempBounds.X, FlightTempBounds.Y)
	//&& EffectiveWeight < FlightWeightCutOff;
}

bool UALSFlightComponent::FlightInterruptCheck_Implementation(AActor* Other, FVector NormalImpulse,	const FHitResult& Hit) const
{
	float MyVelLen;
	FVector MyVelDir;
	OwnerCharacter->GetVelocity().GetAbs().ToDirectionAndLength(MyVelDir, MyVelLen);
	return MyVelLen >= FlightInterruptThreshold;
}