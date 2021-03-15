
#include "PlayerControllerBase.h"

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	HUD = CreateWidget(this, HUDClass);
	if (HUD)
		HUD->AddToViewport();
}
