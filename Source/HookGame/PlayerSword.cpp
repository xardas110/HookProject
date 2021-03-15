// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSword.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIBehaviorBase.h"
// Sets default values
APlayerSword::APlayerSword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SwordHandleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Sword Handle Location"));
	SwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Sword Hitbox"));
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Mesh"));
	
	RootComponent = SwordHandleLocation;

	SwordHitbox->SetupAttachment(SwordHandleLocation);
	SwordMesh->SetupAttachment(SwordHitbox);
	SwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerSword::OnBeginOverlap);
}

// Called when the game starts or when spawned
void APlayerSword::BeginPlay()
{
	Super::BeginPlay();
	SwordHitbox->SetGenerateOverlapEvents(false);
	SwordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerSword::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Something"));
	
	if (OtherActor)
	{
		if (HitRegisteredComponentsInCurrentSwing.Contains(OtherActor))		
			return;
		UE_LOG(LogTemp, Warning, TEXT("Hit accepted"));
		HitRegisteredComponentsInCurrentSwing.Push(OtherActor);
		FDamageEvent DMGEvent;
		const auto PlayerController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
		if (PlayerController)
			OtherActor->TakeDamage(Damage, DMGEvent, PlayerController, this);
	}
}

// Called every frame
void APlayerSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UBoxComponent* APlayerSword::GetCollisionComponent() const
{
	return SwordHitbox;
}

void APlayerSword::StartSwing()
{
	HitRegisteredComponentsInCurrentSwing.Empty();
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SwordHitbox->SetGenerateOverlapEvents(true);
}

void APlayerSword::EndSwing()
{
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordHitbox->SetGenerateOverlapEvents(false);
}

void APlayerSword::ComboSwing()
{
	HitRegisteredComponentsInCurrentSwing.Empty();
}

