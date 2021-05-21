#include "SavingAndLoading.h"

USavingAndLoading::USavingAndLoading()
{
	PlayerLocation = { 0.f,0.f,0.f };
	PlayerRotation = { 0.f,0.f,0.f };
	SavedHealth = 0.f;
	SavedLevel = "";
	NPCFollowStatus = false;
}

void USavingAndLoading::SaveLevelName(FName Level)
{
	FString LevelString = Level.ToString();
	
	if(SavedLevel != LevelString)
	{
		SavedLevel = LevelString;
		/*UE_LOG(LogTemp, Warning, TEXT("The saved level is %s. Went to if."), *SavedLevel)*/
	}
}

void USavingAndLoading::OpenLevel()
{
	UWorld* GameWorld = GetWorld();

	const FString CurrentLevelString = GameWorld->GetName();

	if (SavedLevel != CurrentLevelString)
		UGameplayStatics::OpenLevel(GetWorld(), (TEXT("/Game/Maps/%s"), *SavedLevel));	
}
