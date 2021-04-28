// Fill out your copyright notice in the Description page of Project Settings.


#include "KillPlayerBox.h"
#include "PlayerBase.h"
#include "PlayerControllerBase.h"

// Sets default values
AKillPlayerBox::AKillPlayerBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));

	bHasPlayerOverlapped = false;

}

// Called when the game starts or when spawned
void AKillPlayerBox::BeginPlay()
{
	Super::BeginPlay();

	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &AKillPlayerBox::OnBeginOverlap);
	
}

void AKillPlayerBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor)
	{
		return;
	}

	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{

		UE_LOG(LogTemp, Warning, TEXT("Player overlapped a kill box."))

		bHasPlayerOverlapped = true;

		const auto PlayerReference = Cast<APlayerBase>(OtherActor);

		FDamageEvent DamagableEvent;
		
		if(PlayerReference)
		{

			const auto PlayerControllerRef = PlayerReference->Controller;
			
			UE_LOG(LogTemp, Warning, TEXT("Player hit a kill box and was dealt damage."))

			PlayerReference->TakeDamage(DamageValue, DamagableEvent, PlayerControllerRef, this);
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("A PlayerController was not found"));
		}
		
	}
}

