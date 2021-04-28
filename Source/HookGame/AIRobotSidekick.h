// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIBehaviorBase.h"
#include "VisualLogger/VisualLoggerDebugSnapshotInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "DialogueUI.h"
#include "Subtitles.h"
#include "AIRobotSidekick.generated.h"

/**
 * 
 */
UCLASS()
class HOOKGAME_API AAIRobotSidekick : public AAIBehaviorBase, public IVisualLoggerDebugSnapshotInterface
{
	GENERATED_BODY()
	
public:
	AAIRobotSidekick();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* PropellerBlade{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFollowPlayer = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Robot Sidekick Parameters")
		FVector RangeToGetFrightened{0.f,0.f,0.f};  // The vision range that determines if a monster is close enough to scare the robot

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Sidekick Parameters")
		FVector MinimumDistanceToPlayer{0.f,0.f,0.f} ; // The Minimum distance the robot wants to be from the player. If under, the robot will attempt to move away

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Sidekick Parameters")
		FVector MaximumDistanceToPlayer{0.f,0.f,0.f}; // The maximum distance the robot wants to be form the player. If reached, the robot will speed up to catch up with the player.


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Dialogue system

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* InteractionComp;

	UFUNCTION()
		void InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex);

		void AnswerPlayerCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay);

		UDataTable* GetPlayerLines() { return PlayerLines; }

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "DialogueSystem")
		bool bInteractable = true;

	

	void SetFollowStatus(bool FollowStatus);

protected:

	UPROPERTY(EditAnywhere, Category = "DialogueSystem")
		UDataTable* PlayerLines;

	UPROPERTY(EditAnywhere, Category = "DialogueSystem")
		UDataTable* NPCLines;

	UPROPERTY(VisibleAnywhere)
		UAudioComponent* AudioComp;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
private:

	UFUNCTION()
		void Talk(USoundBase* SFX, TArray<FSubtitle> Subs);
};
