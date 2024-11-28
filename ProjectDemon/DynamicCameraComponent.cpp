// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicCameraComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include"GameFramework/PlayerController.h"
#include"Camera/CameraComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UDynamicCameraComponent::UDynamicCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDynamicCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
void UDynamicCameraComponent::SetupFocus(AActor* Actor, float transitionSpeed)
{
	if (Actor)
	{
		auto T = transitionSpeed;
		FocusActor = Actor;
		//UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(this, T, EViewTargetBlendFunction::VTBlend_Linear, 0.0, true);
		bIsFocused = true;
	}
}
void UDynamicCameraComponent::Exit(float transitionSpeed)
{
	auto T = transitionSpeed;
	auto player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(player, T, EViewTargetBlendFunction::VTBlend_Linear, 0.0, true);
	bIsFocused = false;
}
bool UDynamicCameraComponent::isFocused() const
{
	return bIsFocused;
}

// Called every frame
void UDynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	/*
	if (FocusActor)
	{
		TArray<AActor*> ToIgnore;

		FHitResult HitResult, HitResult1, HitResult2;
		auto player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		ToIgnore.Add(player);
		ToIgnore.Add(FocusActor);
		auto loc = (FocusActor->GetActorLocation() + player->GetActorLocation()) / 2;
		loc.Z = player->GetActorLocation().Z;
		auto FocusActLoc = FocusActor->GetActorLocation();
		FocusActLoc.Z = player->GetActorLocation().Z;
		auto Vect = FocusActLoc - player->GetActorLocation();
		Vect.Normalize();
		auto PlayerToEnemyRightVect = UKismetMathLibrary::GetRightVector(Vect.Rotation());
		
		auto UpVect = player->GetActorUpVector();
		auto PlayerToEnemyLeftVect = -1 * PlayerToEnemyRightVect;
		auto PlayerToEnemyBackVect = -1 * Vect;

		auto Start = player->GetActorLocation();
		float ArrowSize = 250;
		float VectorDebugLength = 250;
		float VectorDebugThickness = 10;
		FLinearColor Color = FLinearColor::Blue;
		if (bEnableDebug)
		{
			UKismetSystemLibrary::DrawDebugArrow(this, Start, Start + PlayerToEnemyRightVect * VectorDebugLength, ArrowSize, Color, 0.0, VectorDebugThickness);
			UKismetSystemLibrary::DrawDebugArrow(this, Start, Start + PlayerToEnemyLeftVect * VectorDebugLength, ArrowSize, Color, 0.0, VectorDebugThickness);
			UKismetSystemLibrary::DrawDebugArrow(this, Start, Start + PlayerToEnemyBackVect * VectorDebugLength, ArrowSize, Color, 0.0, VectorDebugThickness);
			UKismetSystemLibrary::DrawDebugSphere(this, loc, 50.0, 12, FLinearColor::MakeRandomColor());
		}



		auto PlayerToCameraVect = player->GetActorLocation() - GetActorLocation();
		PlayerToCameraVect.Normalize();
		auto CameraRightVect = UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotator(0, 0, GetActorRotation().Yaw));

		Start = loc;
		ArrowSize = 100;
		VectorDebugLength = 250;
		VectorDebugThickness = 10;
		Color = FLinearColor::Green;

		if (bEnableDebug)
		{
			UKismetSystemLibrary::DrawDebugArrow(this, Start, Start - GetActorRightVector() * VectorDebugLength, ArrowSize, Color, 0.0, VectorDebugThickness);
			UKismetSystemLibrary::DrawDebugArrow(this, Start, Start + GetActorRightVector() * VectorDebugLength, ArrowSize, Color, 0.0, VectorDebugThickness);
		}


		auto a = FVector::DotProduct(PlayerToCameraVect, GetActorRightVector());
		auto b = FVector::DotProduct(PlayerToCameraVect, -1 * GetActorRightVector());
		int A = FMath::RoundToInt(a);
		int B = FMath::RoundToInt(b);
		float SideLengthValue = 0.0;
		if ((FVector::Dist(FocusActor->GetActorLocation(), player->GetActorLocation()) < MaxDistanceToFocus) && 1)
		{
			auto Top = MaxDistanceToFocus - FVector::Dist(FocusActor->GetActorLocation(), player->GetActorLocation());
			auto Bottom = MinDistanceToFocus;
			SideLengthValue = Top / Bottom < 1 ? (Top / Bottom) * CameraSideLength : CameraSideLength;
		}
		auto Pos1 = player->GetActorLocation() + (UpVect * CameraUpLength + PlayerToEnemyRightVect * SideLengthValue + PlayerToEnemyBackVect * CameraBackLength);
		auto Pos2 = player->GetActorLocation() + (UpVect * CameraUpLength + PlayerToEnemyLeftVect * SideLengthValue + PlayerToEnemyBackVect * CameraBackLength);
		FVector CameraLoc = a < b ?
			Pos1 :
			Pos2;
		if (bEnableDebug)
		{
			UKismetSystemLibrary::DrawDebugLine(this, player->GetActorLocation(), CameraLoc, FLinearColor::Red);
		}
		auto DrawDebugTrace = bEnableDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;

		bool didHit1 = UKismetSystemLibrary::LineTraceSingle(this, player->GetActorLocation(), Pos1,
			ETraceTypeQuery::TraceTypeQuery2, 1, ToIgnore, DrawDebugTrace, HitResult1, true);
		bool didHit2 = UKismetSystemLibrary::LineTraceSingle(this, player->GetActorLocation(), Pos2,
			ETraceTypeQuery::TraceTypeQuery2, 1, ToIgnore, DrawDebugTrace, HitResult2, true);

		bool didHit = UKismetSystemLibrary::LineTraceSingle(this, player->GetActorLocation(), CameraLoc,
			ETraceTypeQuery::TraceTypeQuery2, 1, ToIgnore, DrawDebugTrace, HitResult, true);
		if (didHit && 0)
		{
			CameraLoc = HitResult.Location;
		}
		if (didHit && 1)
		{
			CameraLoc = HitResult.Location;
			if (CameraLoc.Equals(Pos1))
			{
				if (didHit2)
				{
					auto Dist1 = FVector::Dist(HitResult2.Location, GetActorLocation());
					auto Dist2 = FVector::Dist(HitResult.Location, GetActorLocation());
					if (Dist1 > Dist2)
					{
						CameraLoc = HitResult2.Location;
					}
				}
				else
				{
					CameraLoc = Pos2;
				}
			}
			if (CameraLoc.Equals(Pos2))
			{
				if (didHit1)
				{
					auto Dist1 = FVector::Dist(HitResult1.Location, GetActorLocation());
					auto Dist2 = FVector::Dist(HitResult.Location, GetActorLocation());
					if (Dist1 > Dist2)
					{
						CameraLoc = HitResult1.Location;
					}
				}
				else
				{
					CameraLoc = Pos1;
				}

			}

		}


		if (bEnableDebug)
		{
			UKismetSystemLibrary::DrawDebugLine(this, player->GetActorLocation(), CameraLoc, FLinearColor::Blue);
			UKismetSystemLibrary::DrawDebugSphere(this, CameraLoc, 50.0, 12, FLinearColor::Black);
		}

		auto vloc = UKismetMathLibrary::VInterpTo(GetActorLocation(), CameraLoc, DeltaTime, CameraMovementSpeed);
		SetActorLocation(vloc);
		auto rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), loc);
		SetActorRotation(rot);
	}
	// ...
	*/

}

