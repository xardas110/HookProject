// Fill out your copyright notice in the Description page of Project Settings.


#include "TestAI.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ATestAI::ATestAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("My Collider"));
	RootComponent = MyCollider;
}

// Called when the game starts or when spawned
void ATestAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

