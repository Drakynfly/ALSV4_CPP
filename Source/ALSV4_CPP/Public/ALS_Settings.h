// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/NoExportTypes.h"
#include "ALS_Settings.generated.h"

/**
* Configurable settings for the ALSV4_CPP_Faerie plugin.
*/
UCLASS(Config = "Project", defaultconfig, meta = (DisplayName = "ALS Faerie"))
class ALSV4_CPP_API UALS_Settings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	* This is the channel that the mantle check function will use to determine if something can be mantled.
	* WorldStatic will work fine, or you can create a custom channel for more precise control.
	*/
	UPROPERTY(EditAnywhere, Config, Category = "Mantling")
	TEnumAsByte<ECollisionChannel> MantleCheckChannel;

	/**
	* This channel is used by the flight system to determine the altitude from the ground. WorldStatic works, but if
	* you have a landscape or other large mass that you want to count as ground, then you can change this.
	*/
	UPROPERTY(EditAnywhere, Config, Category = "Flight")
	TEnumAsByte<ECollisionChannel> FlightCheckChannel;

	// Height of sea level, in world space.
	UPROPERTY(EditDefaultsOnly, Category = "Flight")
	float SeaAltitude = 0.f;

	/**
	 * Height of the troposphere. Represents the max altitude that flight can be maintained at. This is in world space,
	 * not relative to sea level. E.g, if you want a 100,000 unit tall atmosphere, but the sea altitude is set to -1,000,
	 * then you need to set this to 99,000.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Flight")
	float TroposphereHeight = 1000000.f;

	static FORCEINLINE UALS_Settings* Get()
	{
		UALS_Settings* Settings = GetMutableDefault<UALS_Settings>();
		check(Settings);

		return Settings;
	}
};