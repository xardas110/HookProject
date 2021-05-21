#include "XpBall.h"
#include "../Character/PlayerBase.h"

AXpBall::AXpBall()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Inner Sphere"));	
	OuterSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Outer Sphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Xp ball mesh"));
	
	RootComponent = InnerSphere;

	Mesh->SetupAttachment(RootComponent);
	OuterSphere->SetupAttachment(RootComponent);

	OuterSphere->OnComponentBeginOverlap.AddDynamic(this, &AXpBall::OnBeginOverlapOuterSphere);
	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &AXpBall::OnBeginOverlapInnerSphere);
}

void AXpBall::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
}

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
	{
		PlayerRef = OtherActor;
		PrimaryActorTick.SetTickFunctionEnable(true);
	}
}

void AXpBall::OnBeginOverlapInnerSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerBase::StaticClass()))
	{
		if (const auto PlayerBase = Cast<APlayerBase>(OtherActor))
		{
			PlayerBase->OnIncomingXP(AmountOfXp);
			this->Destroy();
		}
	}	
}
