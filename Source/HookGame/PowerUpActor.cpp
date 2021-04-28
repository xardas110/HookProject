// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpActor.h"

// Sets default values
APowerUpActor::APowerUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InteractableComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PowerUp Hitbox"));
	RootComponent = InteractableComponent;
	PowerUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerUp Mesh"));
	PowerUpMesh->SetupAttachment(InteractableComponent);
	InteractableComponent->OnComponentBeginOverlap.AddDynamic(this, &APowerUpActor::OnBeginOverlap);
	InteractableComponent->OnComponentEndOverlap.AddDynamic(this, &APowerUpActor::OnEndOverlap);
}


// Called when the game starts or when spawned
void APowerUpActor::BeginPlay()
{
	Super::BeginPlay();
	RespawnTimer = 0.f;
	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();
}

// Called every frame
void APowerUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bInteractable)
		return;
	RespawnTimer += DeltaTime;
	if(RespawnTimer > TimeUntilRespawn)
	{
		SetActorLocation(InitialLocation);
		SetActorRotation(InitialRotation);
		PowerUpMesh->SetVisibility(true);
		InteractableComponent->SetGenerateOverlapEvents(true);
	}
}


void APowerUpActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!OtherActor)
		return;
	
	if(HealthPowerUp)
	{
		
		if(bInteractable)
		{
			
			if(OtherActor->IsA(APlayerBase::StaticClass()))
			{
				const auto PlayerReference = Cast<APlayerBase>(OtherActor);
				if(PlayerReference)
				{
					const auto PlayerControllerRef = PlayerReference->Controller;
					FDamageEvent HealthUpEvent;
					if(PlayerControllerRef)
					{
						PowerUpMesh->SetVisibility(false);
						InteractableComponent->SetGenerateOverlapEvents(false);
						OtherActor->TakeDamage(-AddedPowerValue, HealthUpEvent, GetInstigatorController(), this);
						RespawnTimer = 0.f;
					}		
				}				
			}
		}	
	}
	
	
	
}

void APowerUpActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}