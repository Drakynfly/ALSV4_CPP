// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Doğa Can Yanıkoğlu
// Contributors:

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ALSPlayerController.generated.h"

class AALSPlayerCharacter;

/**
 * Player controller class
 */
UCLASS(Blueprintable, BlueprintType)
class ALSV4_CPP_API AALSPlayerController : public APlayerController
{
	GENERATED_BODY()

	/*
	 * Project Faerie has its own base player controller class, and so cannot use this one. The following code, alongside
	 * the accompanying code in the cpp file, should be copied from here to that class (and integrated as necessary),
	 * and updated if this changes.
	 */

	/*** BEGIN CODE COPIED FROM AALSPLAYERCONTROLLER ***/

public:
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void OnRep_Pawn() override;
	virtual void BeginPlayingState() override;

private:
	void SetupCamera();
	void SetupDebugInputs();

public:
	/** Main character reference */
	UPROPERTY(BlueprintReadOnly, Category = "ALS Player Controller")
	AALSPlayerCharacter* PossessedCharacter = nullptr;

	/*** END CODE COPIED FROM AALSPLAYERCONTROLLER ***/
};