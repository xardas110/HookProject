// Fill out your copyright notice in the Description page of Project Settings.


#include "XpBall.h"
#include "PlayerBase.h"

// Sets default values
AXpBall::AXpBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Inner Sphere"));	
	OuterSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Outer Sphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Xp ball mesh"));
	
	RootComponent = InnerSphere;

	Mesh->SetupAttachment(RootComponent);
	OuterSphere->SetupAttachment(RootComponent);

	OuterSphere->OnComponentBeginOverlap.AddDynamic(this, &AXpBall::OnBeginOverlapOuterSphere);
	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &AXpBall::OnBeginOverlapInnerSphere);
}

// Called when the game starts or when spawned
void AXpBall::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
}

// Called every frame
void AXpBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef)
	{
		const auto MyLocation = GetActorLocation();
		const auto PlayerLocation = PlayerRef->GetActorLocation();
		const auto NewLocation = FMath::Lerp(MyLocation, PlayerLocation, LerpSpeed);
		SetActorLocation(NewLocation);
	}
	
}

void AXpBall::OnBeginOverlapOuterSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerBase::StaticClass()))
		PlayerRef = OtherActor;	
}

void AXpBall::OnBeginOverlapInnerSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerBase::StaticClass()))
	{
		if (auto PlayerBase = Cast<APlayerBase>(OtherActor))
		{
			PlayerBase->OnIncomingXP(AmountOfXp);
			this->Destroy();
		}
	}
	
}
