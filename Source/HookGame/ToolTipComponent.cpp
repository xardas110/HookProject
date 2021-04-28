// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolTipComponent.h"

#include "PlayerBase.h"

// Sets default values
AToolTipComponent::AToolTipComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ToolTipBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ToolTipBox"));
	RootComponent = ToolTipBox;

	bHasPlayerOverlapped = false;
	

}

void AToolTipComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor)
	{
		return;
	}


	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
		bHasPlayerOverlapped = true;

		const auto PlayerReference = Cast<APlayerBase>(OtherActor);

		if(PlayerReference)
		{
			const auto PlayerControllerRef = PlayerReference->Controller;

			if(PlayerControllerRef)
			{
				
			}

			
		}

		else
		{
			UE_LOG(LogTemp, Error, TEXT("A PlayerController was not found"))
		}
		
		
	}
}

// Called when the game starts or when spawned
void AToolTipComponent::BeginPlay()
{
	Super::BeginPlay();

	ToolTipBox->OnComponentBeginOverlap.AddDynamic(this, &AToolTipComponent::OnBeginOverlap);
	
	
}

// Called every frame
void AToolTipComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

