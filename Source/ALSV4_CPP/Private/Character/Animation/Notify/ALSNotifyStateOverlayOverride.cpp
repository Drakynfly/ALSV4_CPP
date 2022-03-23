// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community

#include "Character/Animation/Notify/ALSNotifyStateOverlayOverride.h"
#include "Character/ALSBaseCharacter.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"

void UALSNotifyStateOverlayOverride::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AALSBaseCharacter* Character = Cast<AALSBaseCharacter>(MeshComp->GetOwner()))
	{
		Character->SetOverlayOverrideState(OverlayOverrideState);
	}
}

void UALSNotifyStateOverlayOverride::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AALSBaseCharacter* Character = Cast<AALSBaseCharacter>(MeshComp->GetOwner()))
	{
		// @todo maybe set to a default stored in character instead of 0???
		Character->SetOverlayOverrideState(0);
	}
}

FString UALSNotifyStateOverlayOverride::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Overlay Override State: "));
	Name.Append(FString::FromInt(OverlayOverrideState));
	return Name;
}
