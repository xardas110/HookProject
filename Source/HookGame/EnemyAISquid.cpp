// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAISquid.h"


// Sets default values
AEnemyAISquid::AEnemyAISquid()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

	
}

// Called when the game starts or when spawned
void AEnemyAISquid::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyAISquid::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT(" AI: I ATTACKED! "));
}

// Called every frame
void AEnemyAISquid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void AEnemyAISquid::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

