// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

}
