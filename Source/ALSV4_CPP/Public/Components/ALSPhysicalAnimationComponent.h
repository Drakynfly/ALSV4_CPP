// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "ALSPhysicalAnimationComponent.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class ALSV4_CPP_API UALSPhysicalAnimationComponent : public UPhysicalAnimationComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Physical Animation")
	void EnableByProfile(FName Bone = "root", FName Profile = "None", bool IncludeSelf = true, bool Mirrored = false);

	UFUNCTION(BlueprintCallable, Category = "Physical Animation")
	void ClearPhysicalAnimData() const;

	UFUNCTION(BlueprintCallable, Category = "Physical Animation")
	void ClearPhysicalAnimDataBelow(FName Bone = "root", bool IncludeSelf = true) const;

	UFUNCTION(BlueprintPure, Category = "Physical Animation")
	USkeletalMeshComponent* GetAnimatingMesh() const;
};