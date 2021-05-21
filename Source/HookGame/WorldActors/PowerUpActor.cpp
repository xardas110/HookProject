#include "PowerUpActor.h"
#include "../Helpers/Macros.h"
#include "Components/BoxComponent.h"

APowerUpActor::APowerUpActor()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractableComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PowerUp Hitbox"));
	PowerUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerUp Mesh"));
	GrappleTracer = CreateDefaultSubobject<UGrappleTracer>(TEXT("Grapple tracer plant"));
	
	RootComponent = InteractableComponent;
	PowerUpMesh->SetupAttachment(InteractableComponent);
	
	InteractableComponent->OnComponentBeginOverlap.AddDynamic(this, &APowerUpActor::OnBeginOverlap);
	InteractableComponent->SetMobility(EComponentMobility::Movable);
	InteractableComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
}

void APowerUpActor::BeginPlay()
{
	Super::BeginPlay();
	
	GrappleTracer->ComponentExtent->SetBoxExtent({ 100.f, 100.f, 100.f });
	GrappleTracer->ComponentExtent->SetWorldLocation(GetActorLocation());
	GrappleTracer->GrapplePoint->SetWorldLocation(GetActorLocation());
	
	InteractableComponent->SetMobility(EComponentMobility::Movable);
}

void APowerUpActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ReturnIfNull(OtherActor);
	ReturnIfNull(HealthPowerUp);
	ReturnIfNull(bInteractable);

	if (const auto PlayerReference = Cast<APlayerBase>(OtherActor))
	{
		const FDamageEvent HealthUpEvent;
		InteractableComponent->SetGenerateOverlapEvents(false);
		OtherActor->TakeDamage(-AddedPowerValue, HealthUpEvent, GetInstigatorController(), this);
		this->Destroy();
	}
}
