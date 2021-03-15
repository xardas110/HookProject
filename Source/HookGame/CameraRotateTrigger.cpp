// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraRotateTrigger.h"
#include "PlayerBase.h"
// Sets default values
ACameraRotateTrigger::ACameraRotateTrigger()
{
 	//Never turn this on, might have more than 10 triggers in a level
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Triggerbox"));
}

void ACameraRotateTrigger::ResetToDefault()
{
	Reverse = false;
}

void ACameraRotateTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACameraRotateTrigger::OnBeginOverLap);
	
	if (bOnEndOverlapRotation)
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ACameraRotateTrigger::OnEndOverLap);
}

void ACameraRotateTrigger::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	return;

	if (OtherActor->IsA(APlayerBase::StaticClass()))
	{
		const auto PlayerRef = Cast<APlayerBase>(OtherActor);
		PlayerRef->OnBeginCameraTrigger(this);
	}
}

void ACameraRotateTrigger::OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;

	if (OtherActor->IsA(APlayerBase::StaticClass()))
	{
		const auto PlayerRef = Cast<APlayerBase>(OtherActor);
		PlayerRef->OnEndCameraTrigger(this);
	}
}

