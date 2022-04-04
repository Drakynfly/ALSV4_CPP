// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSBaseCharacter.h"
#include "Components/ActorComponent.h"
#include "ALSFlightComponent.generated.h"


UCLASS(Blueprintable, BlueprintType)
class ALSV4_CPP_API UALSFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UALSFlightComponent();

protected:
	virtual void BeginPlay() override;

public:

	// @TODO flight catching should really be in the movement state machine not in the flight component.
	UFUNCTION()
	void OnOwningMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	UFUNCTION()
	void OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	// Essentially, this is this component's tick function, except it's only called when needed by its owning actor.
	void UpdateFlight(float DeltaTime);

	void UpdateFlightRotation(float DeltaTime);

	float CalculateFlightRotationRate() const;

	void UpdateFlightMovement(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure = false, Category = "ALS|Flight")
	bool FlightInterruptCheck(AActor* Other, FVector NormalImpulse, const FHitResult& Hit) const;

	/** This can be overriden to setup custom conditions for allowing character flight. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "ALS|Flight")
	virtual bool CanFly() const;

	/** This can be overriden to setup custom conditions for allowing character flight from blueprint. */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure = false, Category = "ALS|Flight")
	bool FlightCheck() const;

	// @TODO flight catching should really be in the movement state machine not in the flight component.
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Flight")
	bool WantsToCatchFalling() const;

	// Gets the relative altitude of the player, measuring down to a point below the character.
	UFUNCTION(BlueprintCallable, Category = "ALS|Flight")
	float FlightDistanceCheck(float CheckDistance, FVector Direction) const;

	void AdjustFlightInput(FVector& WorldDirection, float& ScaleValue);

protected:
	// The velocity of the hit required to trigger a positive FlightInterruptThresholdCheck.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Flight")
	float FlightInterruptThreshold = 600;

	// Should FlightInterruptCheck be used to determine if flight should be interrupted when colliding in midair.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Flight")
	bool UseFlightInterrupt = true;

	// Flag to call CanFly() per tick. This will disable flight in midair on a false return. When disabled, CanFly() will
	// only be called on pre-takeoff.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Flight")
	bool AlwaysCheckFlightConditions = false;

	// The max degree that flight input will consider forward.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Flight", Meta = (UIMin = 0, UIMax = 90))
	float MaxFlightForwardAngle = 85;

	// Maximum rotation in Yaw, Pitch and Roll, that may be achieved in flight.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|Flight")
	FVector MaxFlightLean = {40, 40, 0};

	// @TODO flight catching should really be in the movement state machine not in the flight component.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|Flight")
	float TimeToWaitBeforeCatchFalling = 1;

private:
	UPROPERTY()
	TObjectPtr<AALSBaseCharacter> OwnerCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<UALSDebugComponent> ALSDebugComponent = nullptr;

	// Altitude variables for flight calculations.
	float SeaAltitude = 0; // @todo essentially global.

	float AtmosphereAtAltitude = 1;

	float TroposphereHeight = 0; // @todo essentially global.

	float RelativeAltitude = 0;

	// @TODO flight catching should really be in the movement state machine not in the flight component.
	// Used to track how long we have been falling for.
	FDateTime TimeStartingFalling;
};