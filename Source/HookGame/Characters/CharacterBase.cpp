#include "CharacterBase.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
