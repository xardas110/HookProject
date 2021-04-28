// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadNewLevel.h"

// Sets default values
ALoadNewLevel::ALoadNewLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LevelTransferBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LevelTransferBox"));

	bHasPlayerOverlapped = false;

}

// Called when the game starts or when spawned
void ALoadNewLevel::BeginPlay()
{
	Super::BeginPlay();

	LevelTransferBox->OnComponentBeginOverlap.AddDynamic(this, &ALoadNewLevel::OnBeginOverlap);
	
}

void ALoadNewLevel::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor)
	{
		return;
	}


	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
		bHasPlayerOverlapped = true;
	}

}

// Called every frame
void ALoadNewLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

