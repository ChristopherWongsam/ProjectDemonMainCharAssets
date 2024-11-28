// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicCameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UDynamicCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDynamicCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupFocus(class AActor* Enemy, float transitionSpeed = 1.0);
	void Exit(float transitionSpeed = 1.0);
	bool isFocused() const;
	bool bIsFocused;
	//Speed to transiton to new camera.
	// Called to bind functionality to input
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraPosition)
		float DefaultTargetArmLength = 400.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraPosition)
		float CameraUpLength = 250;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraPosition)
		float CameraSideLength = 250;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraPosition)
		float CameraBackLength = 250;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSpeed)
		float CameraMovementSpeed = 2.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = debug)
		bool bEnableDebug;
	//Distance when it should complete 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerDistance)
		float MinDistanceToFocus = 250.0;
	//Distance when it should Should
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerDistance)
		float MaxDistanceToFocus = 600.0;

	AActor* FocusActor;
		
};
