// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "DialogueUI.h"
#include "Subtitles.h"
#include "AIRobotSidekick.h"
#include "NPCActor.generated.h"

UCLASS()
class HOOKGAME_API 
	Actor : public AActor
{
	GENERATED_BODY()
	
//public:
//	
//	// Sets default values for this actor's properties
//	ANPCActor();
//
//	UPROPERTY(EditAnywhere, Category = "Setup");
//	USkeletalMeshComponent* NPCMesh = nullptr;
//	
//	UPROPERTY(EditAnywhere, Category = "Setup");
//	USkeletalMeshComponent* NPCMesh2 = nullptr;
//
//
//	UPROPERTY(VisibleAnywhere)
//	UCapsuleComponent* InteractionComp;
//
//	UFUNCTION()
//	void InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);
//
//	UFUNCTION()
//	void InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex);
//
//	void AnswerPlayerCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay);
//
//	UDataTable* GetPlayerLines() { return PlayerLines; }
//
//	UPROPERTY(EditAnywhere)
//	bool bSpawnedNPC = false;
//	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//	
//protected:
//
//	UPROPERTY(EditAnywhere, Category = DialogueSystem)
//	UDataTable* PlayerLines;
//	
//	UPROPERTY(EditAnywhere, Category = DialogueSystem)
//	UDataTable* NPCLines;
//
//	UPROPERTY(VisibleAnywhere)
//	UAudioComponent* AudioComp;
//	
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	UPROPERTY(EditAnywhere, Category = "Setup")
//	TSubclassOf<class AAIRobotSidekick> SpawnableNPC = nullptr;
//
//private:
//
//	UFUNCTION()
//	void Talk(USoundBase* SFX, TArray<FSubtitle> Subs);

};
