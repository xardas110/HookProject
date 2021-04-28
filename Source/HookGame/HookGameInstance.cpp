// Fill out your copyright notice in the Description page of Project Settings.


#include "HookGameInstance.h"
#include "SavingAndLoading.h"
#include "PlayerBase.h"


#define _DEBUG_GAMEINSTANCE
#define _DEBUG_CHECKPOINTGAMEINSTANCE

UHookGameInstance::UHookGameInstance()
{
}

void UHookGameInstance::SaveGameData(AActor* Player)
{

	FString OpenLevel = GetWorld()->GetName();
	FName OpenLevelName = FName(*OpenLevel);

	//Creation of save game instance
	const auto SaveGameInstance = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	if (SaveGameInstance)
	{
#ifdef _DEBUG_GAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameInstanceFound"))
#endif
		SaveGameInstance->SaveLevelName(OpenLevelName);
		const auto PlayerReference = Cast<APlayerBase>(Player);
		if (PlayerReference)
		{
#ifdef _DEBUG_GAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("PlayerCharaceterFound"))
#endif
			SaveGameInstance->PlayerLocation = PlayerReference->GetActorLocation();
			SaveGameInstance->SavedHealth = PlayerReference->CurrentHealth;
			SaveGameInstance->NPCFollowStatus = PlayerReference->bNPCFollowPlayer;
			
			SaveGameInstance->SkillTreeLevel = PlayerReference->SkillTreeFCRI->Level;
			SaveGameInstance->CurrentExperience = PlayerReference->SkillTreeFCRI->CurrentExperience;
			SaveGameInstance->SkillPoints = PlayerReference->SkillTreeFCRI->SkillPoints;
			
			UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySaveGameSlot"), 0);
	
			//Log message to let the player know the game is saved.
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Saved."));
			
	#ifdef _DEBUG_GAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("Saved level is %s"), *SaveGameInstance->SavedLevel)
	#endif
		}
	}
}

void UHookGameInstance::LoadGameData(AActor* Player)
{
	bool bDoesSaveGameExist = false;	
	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));
	
	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MySaveGameSlot", 0);
	if (bDoesSaveGameExist)
	{
		#ifdef _DEBUG_GAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameDoesExist"))
		#endif
		LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MySaveGameSlot", 0));
		if (LoadGameSlot)
		{
			#ifdef _DEBUG_GAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("LoadGameSlotRefFound"))
#endif
			UGameplayStatics::OpenLevel(GetWorld(), (TEXT("/Game/Maps/%s"), *LoadGameSlot->SavedLevel), TRAVEL_Absolute);

			bLevelIsLoaded = true;
			bNPCDataLoad = true;

			if (bLevelIsLoaded)
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game map Loaded."));
		}
	}
}

void UHookGameInstance::LoadPlayerData(APlayerBase* PlayerBase)
{	
	bool bDoesSaveGameExist = false;
	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MySaveGameSlot", 0);
	if (bDoesSaveGameExist)
	{
#ifdef _DEBUG_GAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameDoesExist"))
#endif
		LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MySaveGameSlot", 0));	
		if (LoadGameSlot)
		{

			#ifdef _DEBUG_GAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("LOAD: PlayerRefFound"))
			#endif

			PlayerBase->CurrentHealth = LoadGameSlot->SavedHealth;
			PlayerBase->SetActorLocation(LoadGameSlot->PlayerLocation);
			PlayerBase->bNPCFollowPlayer = LoadGameSlot->NPCFollowStatus;
			
			PlayerBase->SkillTreeFCRI->Level = LoadGameSlot->SkillTreeLevel;
			PlayerBase->SkillTreeFCRI->CurrentExperience = LoadGameSlot->CurrentExperience;
			PlayerBase->SkillTreeFCRI->SkillPoints = LoadGameSlot->SkillPoints;
			
			#ifdef _DEBUG_GAMEINSTANCE
			FString PlayerLocation = LoadGameSlot->PlayerLocation.ToString();
			UE_LOG(LogTemp, Warning, TEXT("PlayerLocation saved is: %s"), *PlayerLocation);
			#endif
		}
	}
}

void UHookGameInstance::LoadNPCData(AAIRobotSidekick* Robot)
{
	bool bDoesSaveGameExist = false;
	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MySaveGameSlot", 0);
	if (bDoesSaveGameExist)
	{
#ifdef _DEBUG_GAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameDoesExist"))
#endif
		LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MySaveGameSlot", 0));
		if (LoadGameSlot)
		{

#ifdef _DEBUG_GAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("LOAD: PlayerRefFound"))
#endif

			if(LoadGameSlot->NPCFollowStatus)
			{
				
				FVector RobotSpawn= { 100.f,100.f,100.f };

				FVector PlayerLocation = LoadGameSlot->PlayerLocation;

				Robot->SetActorLocation(PlayerLocation+RobotSpawn);
				Robot->bInteractable = false;
				Robot->bFollowPlayer = true;


	#ifdef _DEBUG_GAMEINSTANCE
				FString RobotLocation = (PlayerLocation+RobotSpawn).ToString();
				UE_LOG(LogTemp, Warning, TEXT("RobotLocation saved is: %s"), *RobotLocation);
	#endif
			}
		}
	}
}

void UHookGameInstance::SaveLatestCheckpoint(AActor* Player)
{

	FString OpenLevel = GetWorld()->GetName();
	FName OpenLevelName = FName(*OpenLevel);

	//Creation of save game instance
	const auto SaveGameInstance = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	if (SaveGameInstance)
	{
#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameInstanceFound"))
#endif

			SaveGameInstance->SaveLevelName(OpenLevelName);

		const auto PlayerReference = Cast<APlayerBase>(Player);

		if (PlayerReference)
		{
#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("Checkpoint: PlayerCharaceterFound"))
#endif

			SaveGameInstance->PlayerLocation = PlayerReference->GetActorLocation();
			SaveGameInstance->SavedHealth = PlayerReference->CurrentHealth;
			SaveGameInstance->NPCFollowStatus = PlayerReference->bNPCFollowPlayer;
			
			SaveGameInstance->SkillTreeLevel = PlayerReference->SkillTreeFCRI->Level;
			SaveGameInstance->CurrentExperience = PlayerReference->SkillTreeFCRI->CurrentExperience;
			SaveGameInstance->SkillPoints = PlayerReference->SkillTreeFCRI->SkillPoints;

			UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MyCheckpointSlot"), 1);

			//Log message to let the player know the checkpoint is reached and saved.
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Checkpoint reached."));

#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("Checkpoint: Saved level is %s"), *SaveGameInstance->SavedLevel)
#endif
		}
	}
}

void UHookGameInstance::LoadLatestCheckpoint(AActor* Player)
{
	bool bDoesSaveGameExist = false;

	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MyCheckpointSlot", 1);

	if (bDoesSaveGameExist)
	{

#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("CheckPointSaveDoesExist"))
#endif
		LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MyCheckpointSlot", 1));
		if (LoadGameSlot)
		{

#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("LoadGameSlotRefFound"))
#endif

				UGameplayStatics::OpenLevel(GetWorld(), (TEXT("/Game/Maps/%s"), *LoadGameSlot->SavedLevel), TRAVEL_Absolute);

			bCheckpointIsLoaded = true;
			bNPCCheckpointDataLoad = true;

			if (bCheckpointIsLoaded)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("bCheckpointIsLoaded."));
			}

		}
	}	
}

void UHookGameInstance::LoadPlayerCheckpointData(APlayerBase* PlayerBase)
{
	bool bDoesSaveGameExist = false;

	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MyCheckpointSlot", 1);
	if (bDoesSaveGameExist)
	{
#ifdef _DEBUG_GAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameDoesExist"))
#endif
		LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MyCheckpointSlot", 1));
		if (LoadGameSlot)
		{

#ifdef _DEBUG_GAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("LOAD: PlayerRefFound"))
#endif

			PlayerBase->CurrentHealth = LoadGameSlot->SavedHealth;
			PlayerBase->SetActorLocation(LoadGameSlot->PlayerLocation);
			PlayerBase->bNPCFollowPlayer = LoadGameSlot->NPCFollowStatus;

			PlayerBase->SkillTreeFCRI->Level = LoadGameSlot->SkillTreeLevel;
			PlayerBase->SkillTreeFCRI->CurrentExperience = LoadGameSlot->CurrentExperience;
			PlayerBase->SkillTreeFCRI->SkillPoints = LoadGameSlot->SkillPoints;

#ifdef _DEBUG_GAMEINSTANCE
			FString PlayerLocation = LoadGameSlot->PlayerLocation.ToString();
			UE_LOG(LogTemp, Warning, TEXT("PlayerLocation saved is: %s"), *PlayerLocation);
#endif
		}
	}	
}

void UHookGameInstance::LoadNPCCheckpoint(AAIRobotSidekick* Robot)
{
	bool bDoesSaveGameExist = false;

	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MyCheckpointSlot", 1);

	if (bDoesSaveGameExist)
	{

#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
		UE_LOG(LogTemp, Warning, TEXT("SaveGameDoesExist"))
#endif
		LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MyCheckpointSlot", 1));
		if (LoadGameSlot)
		{

#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
			UE_LOG(LogTemp, Warning, TEXT("LOAD: PlayerRefFound"))
#endif
			if (LoadGameSlot->NPCFollowStatus)
			{

				FVector RobotSpawn = { 100.f,100.f,100.f };
				FVector PlayerLocation = LoadGameSlot->PlayerLocation;

				Robot->SetActorLocation(PlayerLocation + RobotSpawn);
				Robot->bInteractable = false;
				Robot->bFollowPlayer = true;
					
#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
				FString RobotLocation = (PlayerLocation + RobotSpawn).ToString();
				UE_LOG(LogTemp, Warning, TEXT("RobotLocation saved is: %s"), *RobotLocation);
#endif
				/*	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player data Loaded."));*/
			}
		}
	}
}
