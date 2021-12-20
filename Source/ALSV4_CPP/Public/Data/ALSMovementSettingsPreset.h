// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Library/ALSCharacterStructLibrary.h"

#include "ALSMovementSettingsPreset.generated.h"

/**
 *
 */
UCLASS(CollapseCategories)
class ALSV4_CPP_API UALSMovementSettingsPreset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FALSMovementStanceSettings VelocityDirection;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FALSMovementStanceSettings LookingDirection;

	UPROPERTY(EditAnywhere, Category = "Movement Settings")
	FALSMovementStanceSettings Aiming;
};