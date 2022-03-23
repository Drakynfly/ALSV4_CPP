// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/Animation/Notify/ALSAnimNotifyCameraShake.h"


void UALSAnimNotifyCameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (IsValid(OwnerPawn))
	{
		APlayerController* OwnerController = Cast<APlayerController>(OwnerPawn->GetController());
		if (IsValid(OwnerController))
		{
			OwnerController->ClientStartCameraShake(ShakeClass, Scale);
		}
	}
}