#include "HitableMesh.h"
#include "Components/StaticMeshComponent.h"

AHitableMesh::AHitableMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hitable Mesh"));
}

void AHitableMesh::HandleDestruction()
{
	this->Destroy();
}

void AHitableMesh::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetGenerateOverlapEvents(true);
}

void AHitableMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AHitableMesh::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float IncomingDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= FMath::Min(IncomingDamage, Health);
	UE_LOG(LogTemp, Warning, TEXT("Incoming damge"));
	if (Health <= 0.f)
		HandleDestruction();
	return IncomingDamage;
}

