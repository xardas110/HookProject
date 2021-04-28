// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitableMesh.generated.h"

UCLASS()
class HOOKGAME_API AHitableMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitableMesh();

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float Health{ 10.f };

	void HandleDestruction();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
