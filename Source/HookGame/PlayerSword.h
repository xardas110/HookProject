// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <utility>
#include "PlayerSword.generated.h"

UCLASS()
class HOOKGAME_API APlayerSword : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerSword();
private:
	//** Making sure each actor hit is only hit once in current swing**//
	TArray<AActor*> HitRegisteredComponentsInCurrentSwing;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Setup")
	UStaticMeshComponent* SwordMesh{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Setup")
	class UBoxComponent* SwordHitbox{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Setup")
	USceneComponent* SwordHandleLocation{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Setup")
	float Damage = 5.f;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class UBoxComponent* GetCollisionComponent() const;
	void StartSwing();
	void EndSwing();
	void ComboSwing();
};
