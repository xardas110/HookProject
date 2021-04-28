// Fill out your copyright notice in the Description page of Project Settings.


#include "HitableMesh.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
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

// Called when the game starts or when spawned
void AHitableMesh::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetGenerateOverlapEvents(true);
}

// Called every frame
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

