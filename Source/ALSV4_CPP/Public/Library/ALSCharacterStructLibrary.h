// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Doğa Can Yanıkoğlu
// Contributors:


#pragma once

#include "CoreMinimal.h"

#include "Curves/CurveVector.h" // @todo remove

#include "Engine/DataTable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "Library/ALSCharacterEnumLibrary.h"

#include "ALSCharacterStructLibrary.generated.h"

class UCurveVector;
class UAnimMontage;
class UAnimSequenceBase;
class UCurveFloat;
class UNiagaraSystem;
class UMaterialInterface;
class USoundBase;
class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct FALSComponentAndTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FTransform Transform;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	class UPrimitiveComponent* Component = nullptr;
};

USTRUCT(BlueprintType)
struct FALSCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Camera")
	float TargetArmLength = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FVector SocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float LagSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float RotationLagSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool bDoCollisionTest = true;
};

USTRUCT(BlueprintType)
struct FALSCameraGaitSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraSettings Walking;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraSettings Running;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraSettings Sprinting;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraSettings Crouching;
};

USTRUCT(BlueprintType)
struct FALSCameraStateSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraGaitSettings VelocityDirection;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraGaitSettings LookingDirection;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FALSCameraGaitSettings Aiming;
};

USTRUCT(BlueprintType)
struct FALSMantleAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	UCurveVector* PositionCorrectionCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	FVector StartingOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float LowHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float LowPlayRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float LowStartPosition = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float HighHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float HighPlayRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float HighStartPosition = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSMantleParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	UCurveVector* PositionCorrectionCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float StartingPosition = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	FVector StartingOffset = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FALSMantleTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float MaxLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float MinLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float ReachDistance = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float ForwardTraceRadius = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mantle System")
	float DownwardTraceRadius = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	float WalkSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	float RunSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	float SprintSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	UCurveVector* MovementCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	UCurveFloat* RotationRateCurve = nullptr;

	float GetSpeedForGait(const EALSGait Gait) const
	{
		switch (Gait)
		{
		case EALSGait::Running:
			return RunSpeed;
		case EALSGait::Sprinting:
			return SprintSpeed;
		case EALSGait::Walking:
			return WalkSpeed;
		default:
			return RunSpeed;
		}
	}
};

USTRUCT(BlueprintType)
struct FALSMovementStanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FALSMovementSettings Standing;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FALSMovementSettings Crouching;

	// These are not stances, per se, but they fix the best here. I do not like this, but the other option is to have a
	// new struct or many more entries in FALSMovementSettings. I consider this a workable solution only becuase
	// this struct isnt used anywhere else.

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementSettings Flying;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementSettings Swimming;
};

USTRUCT(BlueprintType)
struct FALSRotateInPlaceAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Rotation System")
	UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
	FName SlotName;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
	float SlowTurnRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
	float FastTurnRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
	float SlowPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation System")
	float FastPlayRate = 1.0f;
};

USTRUCT(BlueprintType)
struct FALSHitFX : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Surface")
	TEnumAsByte<enum EPhysicalSurface> SurfaceType;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	EALSSpawnType SoundSpawnType = EALSSpawnType::Attached;

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (EditCondition = "SoundSpawnType == EALSSpawnType::Attached"))
	TEnumAsByte<enum EAttachLocation::Type> SoundAttachmentType;

	UPROPERTY(EditAnywhere, Category = "Sound")
	FVector SoundLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Sound")
	FRotator SoundRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Decal")
	TSoftObjectPtr<UMaterialInterface> DecalMaterial;

	UPROPERTY(EditAnywhere, Category = "Decal")
	EALSSpawnType DecalSpawnType = EALSSpawnType::Attached;

	UPROPERTY(EditAnywhere, Category = "Decal", meta = (EditCondition = "DecalSpawnType == EALSSpawnType::Attached"))
	TEnumAsByte<enum EAttachLocation::Type> DecalAttachmentType;

	UPROPERTY(EditAnywhere, Category = "Decal")
	float DecalLifeSpan = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Decal")
	FVector DecalSize = FVector::OneVector;

	UPROPERTY(EditAnywhere, Category = "Decal")
	FVector DecalLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Decal")
	FRotator DecalRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TSoftObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	EALSSpawnType NiagaraSpawnType = EALSSpawnType::Attached;

	UPROPERTY(EditAnywhere, Category = "Niagara", meta = (EditCondition = "NiagaraSpawnType == EALSSpawnType::Attached"))
	TEnumAsByte<enum EAttachLocation::Type> NiagaraAttachmentType;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	FVector NiagaraLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	FRotator NiagaraRotationOffset = FRotator::ZeroRotator;
};


USTRUCT(BlueprintType)
struct FALSMovementModifier
{
	GENERATED_BODY()

	FALSMovementModifier()
	{
	}

	FALSMovementModifier(const FName ModifierID)
	  : ModifierID(ModifierID)
	{
	}

private:
	FVector Values = FVector::ZeroVector;

public:
	UPROPERTY(EditAnywhere, Category = "Movement Modifier")
	FName ModifierID;

	UPROPERTY(EditAnywhere, Category = "Movement Modifier")
	TObjectPtr<UCurveVector> Affect = nullptr;

	void SetTime(const float Time)
	{
		Values = Affect->GetVectorValue(Time);
	}

	void ApplyModifier(FALSMovementSettings& Settings, const EALSMovementState MovementState)
	{
		if (MovementState == EALSMovementState::Grounded)
		{
			Settings.WalkSpeed *= Values.X;
			Settings.RunSpeed *= Values.X;
			Settings.SprintSpeed *= Values.X;
		}
		else if (MovementState == EALSMovementState::Flight)
		{
			Settings.WalkSpeed *= Values.Y;
            Settings.RunSpeed *= Values.Y;
            Settings.SprintSpeed *= Values.Y;
		}
		else if (MovementState == EALSMovementState::Swimming)
		{
			Settings.WalkSpeed *= Values.Z;
			Settings.RunSpeed *= Values.Z;
			Settings.SprintSpeed *= Values.Z;
		}
	}

	friend bool operator==(const FALSMovementModifier& Lhs, const FALSMovementModifier& RHS)
	{
		return Lhs.ModifierID == RHS.ModifierID;
	}

	friend bool operator!=(const FALSMovementModifier& Lhs, const FALSMovementModifier& RHS)
	{
		return !(Lhs == RHS);
	}

	bool IsValid() const
	{
		return !Affect.IsNull();
	}
};
