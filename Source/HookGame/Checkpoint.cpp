// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckPointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointBox"));
	bHasPlayerOverlapped = false;
	
}

void ACheckpoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	if (OtherActor->IsA(APlayerBase::StaticClass()))
	{

		UE_LOG(LogTemp, Warning, TEXT("Player reached a checkpoint"))

		bHasPlayerOverlapped = true;

		const auto PlayerReference = Cast<APlayerBase>(OtherActor);

		if (PlayerReference)
		{
			PlayerReference->SaveLatestCheckpoint();


		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("A PlayerController was not found"));
		}

	}
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	CheckPointBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBeginOverlap);
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

