// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "ALSExternalMovementInput.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UALSExternalMovementInput : public UInterface
{
	GENERATED_BODY()
};

class IALSExternalMovementInput
{
	GENERATED_BODY()
public:

	// An external force that moves an actor against its will, e.g, strong wind, or mind control.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces|External Input")
	void ExternalInputForce(FVector Direction, float Scale);

	// An external force that does not have to be obeyed fully, e.g, light wind, or rider's commands.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces|External Input")
	void ExternalInputSuggestion(FVector Direction, float Scale);
};