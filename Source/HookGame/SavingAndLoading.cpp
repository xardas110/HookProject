// Fill out your copyright notice in the Description page of Project Settings.


#include "SavingAndLoading.h"

USavingAndLoading::USavingAndLoading()
{

	PlayerLocation = { 0.f,0.f,0.f };
	PlayerRotation = { 0.f,0.f,0.f };
	SavedHealth = 0.f;
	SavedLevel = "";
	NPCFollowStatus = false;
}


//Needs some work.

void USavingAndLoading::SaveLevelName(FName Level)
{

	FString LevelString = Level.ToString();
	
	if(SavedLevel != LevelString)
	{
		SavedLevel = LevelString;

		UE_LOG(LogTemp, Warning, TEXT("The saved level is %s. Went to if."), *SavedLevel)
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The saved level is %s. Went to else"), *SavedLevel)
	}

}

void USavingAndLoading::OpenLevel()
{

	UWorld* GameWorld = GetWorld();

	FString CurrentLevelString = GameWorld->GetName();

	if (SavedLevel == CurrentLevelString)
	{
		UE_LOG(LogTemp, Warning, TEXT("YOU ARE IN THE CURRENT LEVEL"))
	}

	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), (TEXT("/Game/Maps/%s"), *SavedLevel));
	}
	
}
