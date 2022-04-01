// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.


#include "Components/ALSPhysicalAnimationComponent.h"

void UALSPhysicalAnimationComponent::EnableByProfile(const FName Bone, const FName Profile, const bool IncludeSelf,
														const bool Mirrored)
{
	if (!IsValid(GetAnimatingMesh()))
	{
		UE_LOG(LogTemp, Error, TEXT("UALSPhysicalAnimationComponent was not assigned an animating mesh when requested to enable profile!"))
		return;
	}

	FName LocalBone = Bone;
	if (Mirrored)
	{
		FString BoneString = Bone.ToString();
		if (BoneString.EndsWith("_l", ESearchCase::IgnoreCase))
		{
			ApplyPhysicalAnimationProfileBelow(LocalBone, Profile, IncludeSelf, true);
			GetSkeletalMesh()->SetAllBodiesBelowSimulatePhysics(LocalBone, true, IncludeSelf);
			BoneString = BoneString.LeftChop(1).Append("r");
			LocalBone = FName(*BoneString);
		}
		else if (BoneString.EndsWith("_r", ESearchCase::IgnoreCase))
		{
			ApplyPhysicalAnimationProfileBelow(LocalBone, Profile, IncludeSelf, true);
			GetSkeletalMesh()->SetAllBodiesBelowSimulatePhysics(LocalBone, true, IncludeSelf);
			BoneString = BoneString.LeftChop(1).Append("l");
			LocalBone = FName(*BoneString);
		}
		else
		{
			BoneString = BoneString.Append("_r");
			LocalBone = FName(*BoneString);
			ApplyPhysicalAnimationProfileBelow(LocalBone, Profile, IncludeSelf, true);
			GetSkeletalMesh()->SetAllBodiesBelowSimulatePhysics(LocalBone, true, IncludeSelf);
			BoneString = BoneString.LeftChop(1).Append("l");
			LocalBone = FName(*BoneString);
		}
	}
	ApplyPhysicalAnimationProfileBelow(LocalBone, Profile, IncludeSelf, true);
	GetSkeletalMesh()->SetAllBodiesBelowSimulatePhysics(LocalBone, true, IncludeSelf);
}

void UALSPhysicalAnimationComponent::ClearPhysicalAnimData() const
{
	if (GetAnimatingMesh())
	{
		ClearPhysicalAnimDataBelow("root", true);
	}
}

void UALSPhysicalAnimationComponent::ClearPhysicalAnimDataBelow(const FName Bone, const bool IncludeSelf) const
{
	GetSkeletalMesh()->SetAllBodiesBelowSimulatePhysics(Bone, false, IncludeSelf);
	//GetSkeletalMesh()->SetAllBodiesBelowPhysicsBlendWeight(Bone, 0.f);
}

USkeletalMeshComponent* UALSPhysicalAnimationComponent::GetAnimatingMesh() const
{
	return GetSkeletalMesh();
}