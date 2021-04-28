// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthRespawner.h"

// Sets default values
AHealthRespawner::AHealthRespawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHealthRespawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealthRespawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

