﻿// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community

#include "Components/ALSDebugComponent.h"


#include "Character/ALSBaseCharacter.h"
#include "Character/ALSPlayerCameraManager.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

bool UALSDebugComponent::bDebugView = false;
bool UALSDebugComponent::bShowTraces = false;
bool UALSDebugComponent::bShowDebugShapes = false;
bool UALSDebugComponent::bShowLayerColors = false;

UALSDebugComponent::UALSDebugComponent()
{
#if UE_BUILD_SHIPPING
	PrimaryComponentTick.bCanEverTick = false;
#else
	PrimaryComponentTick.bCanEverTick = true;
#endif
}

void UALSDebugComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if !UE_BUILD_SHIPPING
	if (!OwnerCharacter)
	{
		return;
	}

	if (bNeedsColorReset)
	{
		bNeedsColorReset = false;
		SetResetColors();
	}

	if (bShowLayerColors)
	{
		UpdateColoringSystem();
	}
	else
	{
		bNeedsColorReset = true;
	}

	if (bShowDebugShapes)
	{
		DrawDebugSpheres();

		const APlayerController* Controller = Cast<APlayerController>(OwnerCharacter->GetController());
		if (IsValid(Controller))
		{
			AALSPlayerCameraManager* CamManager = Cast<AALSPlayerCameraManager>(Controller->PlayerCameraManager);
			if (IsValid(CamManager))
			{
				CamManager->DrawDebugTargets(OwnerCharacter->GetThirdPersonPivotTarget().GetLocation());
			}
		}
	}
#endif
}

void UALSDebugComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	// Keep static values false on destroy
	bDebugView = false;
	bShowTraces = false;
	bShowDebugShapes = false;
	bShowLayerColors = false;
}

void UALSDebugComponent::FocusedDebugCharacterCycle(const bool bValue)
{
	// Refresh list, so we can also debug runtime spawned characters & remove despawned characters back
	DetectDebuggableCharactersInWorld();

	if (FocusedDebugCharacterIndex == INDEX_NONE)
	{
		// Return here as no AALSBaseCharacter where found during call of BeginPlay.
		// Moreover, for safety set also no focused debug character.
		DebugFocusCharacter = nullptr;
		return;
	}

	if (bValue)
	{
		FocusedDebugCharacterIndex++;
		if (FocusedDebugCharacterIndex >= AvailableDebugCharacters.Num())
		{
			FocusedDebugCharacterIndex = 0;
		}
	}
	else
	{
		FocusedDebugCharacterIndex--;
		if (FocusedDebugCharacterIndex < 0)
		{
			FocusedDebugCharacterIndex = AvailableDebugCharacters.Num() - 1;
		}
	}

	DebugFocusCharacter = AvailableDebugCharacters[FocusedDebugCharacterIndex];
}

void UALSDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AALSBaseCharacter>(GetOwner());
	DebugFocusCharacter = OwnerCharacter;
	if (OwnerCharacter)
	{
		SetDynamicMaterials();
		SetResetColors();
	}
}

void UALSDebugComponent::DetectDebuggableCharactersInWorld()
{
	// Get all ALSBaseCharacter's, which are currently present to show them later in the ALS HUD for debugging purposes.
	TArray<AActor*> AlsBaseCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AALSBaseCharacter::StaticClass(), AlsBaseCharacters);

	AvailableDebugCharacters.Empty();
	if (AlsBaseCharacters.Num() > 0)
	{
		AvailableDebugCharacters.Reserve(AlsBaseCharacters.Num());
		for (AActor* Character : AlsBaseCharacters)
		{
			if (AALSBaseCharacter* AlsBaseCharacter = Cast<AALSBaseCharacter>(Character))
			{
				AvailableDebugCharacters.Add(AlsBaseCharacter);
			}
		}

		FocusedDebugCharacterIndex = AvailableDebugCharacters.Find(DebugFocusCharacter);
		if (FocusedDebugCharacterIndex == INDEX_NONE && AvailableDebugCharacters.Num() > 0)
		{ // seems to be that this component was not attached to and AALSBaseCharacter,
			// therefore the index will be set to the first element in the array.
			FocusedDebugCharacterIndex = 0;
		}
	}
}

void UALSDebugComponent::ToggleGlobalTimeDilationLocal(const float TimeDilation)
{
	if (UKismetSystemLibrary::IsStandalone(this))
	{
		UGameplayStatics::SetGlobalTimeDilation(this, TimeDilation);
	}
}

void UALSDebugComponent::ToggleSlomo()
{
	bSlomo = !bSlomo;
	ToggleGlobalTimeDilationLocal(bSlomo ? 0.15f : 1.f);
}

void UALSDebugComponent::ToggleDebugView()
{
	bDebugView = !bDebugView;

	const AALSPlayerCameraManager* CamManager = Cast<AALSPlayerCameraManager>(
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
	if (IsValid(CamManager))
	{
		UALSPlayerCameraBehavior* CameraBehavior = Cast<UALSPlayerCameraBehavior>(
			CamManager->CameraBehavior->GetAnimInstance());
		if (IsValid(CameraBehavior))
		{
			CameraBehavior->bDebugView = bDebugView;
		}
	}
}

void UALSDebugComponent::OpenOverlayMenu_Implementation(bool bValue)
{
}

void UALSDebugComponent::OverlayMenuCycle_Implementation(bool bValue)
{
}

void UALSDebugComponent::ToggleDebugMesh()
{
	if (bDebugMeshVisible)
	{
		OwnerCharacter->SetVisibleMesh(DefaultSkeletalMesh);
	}
	else
	{
		DefaultSkeletalMesh = OwnerCharacter->GetMesh()->SkeletalMesh;
		OwnerCharacter->SetVisibleMesh(DebugSkeletalMesh);
	}
	bDebugMeshVisible = !bDebugMeshVisible;
}


/** Util for drawing result of single line trace  */
void UALSDebugComponent::DrawDebugLineTraceSingle(const UWorld* World,
                                                  const FVector& Start,
                                                  const FVector& End,
                                                  const EDrawDebugTrace::Type DrawDebugType,
                                                  const bool bHit,
                                                  const FHitResult& OutHit,
                                                  const FLinearColor TraceColor,
                                                  const FLinearColor TraceHitColor,
                                                  const float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

void UALSDebugComponent::DrawDebugCapsuleTraceSingle(const UWorld* World,
	                                                 const FVector& Start,
	                                                 const FVector& End,
	                                                 const FCollisionShape& CollisionShape,
	                                                 const EDrawDebugTrace::Type DrawDebugType,
	                                                 const bool bHit,
	                                                 const FHitResult& OutHit,
	                                                 const FLinearColor TraceColor,
	                                                 const FLinearColor TraceHitColor,
	                                                 const float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			DrawDebugCapsule(World, Start, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(), FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugCapsule(World, OutHit.Location, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(), FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugLine(World, Start, OutHit.Location, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);

			DrawDebugCapsule(World, End, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(), FQuat::Identity, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugLine(World, OutHit.Location, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			DrawDebugCapsule(World, Start, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(), FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugCapsule(World, End, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(), FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

static void DrawDebugSweptSphere(const UWorld* InWorld,
	                        FVector const& Start,
	                        FVector const& End,
	                        const float Radius,
	                        FColor const& Color,
	                        const bool bPersistentLines = false,
	                        const float LifeTime = -1.f,
	                        const uint8 DepthPriority = 0)
{
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;
	float const HalfHeight = (Dist * 0.5f) + Radius;

	FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	DrawDebugCapsule(InWorld, Center, HalfHeight, Radius, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);
}

void UALSDebugComponent::DrawDebugSphereTraceSingle(const UWorld* World,
	                                                  const FVector& Start,
	                                                  const FVector& End,
	                                                  const FCollisionShape& CollisionShape,
	                                                  const EDrawDebugTrace::Type DrawDebugType,
	                                                  const bool bHit,
	                                                  const FHitResult& OutHit,
	                                                  const FLinearColor TraceColor,
	                                                  const FLinearColor TraceHitColor,
	                                                  const float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		const bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		const float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			DrawDebugSweptSphere(World, Start, OutHit.Location, CollisionShape.GetSphereRadius(), TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugSweptSphere(World, OutHit.Location, End, CollisionShape.GetSphereRadius(), TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			DrawDebugSweptSphere(World, Start, End, CollisionShape.GetSphereRadius(), TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}