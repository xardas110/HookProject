#include "HookGameInstance.h"
#include "../Components/SavingAndLoading.h"
#include "../Character/PlayerBase.h"
#include "../Helpers/Macros.h"

//#define _DEBUG_GAMEINSTANCE
//#define _DEBUG_CHECKPOINTGAMEINSTANCE

UHookGameInstance::UHookGameInstance()
{
}

void UHookGameInstance::SaveGameData(AActor* Player)
{
	const FString OpenLevel = GetWorld()->GetName();
	const FName OpenLevelName = FName(*OpenLevel);

	//Creation of save game instance
	const auto SaveGameInstance = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));
	ReturnIfNull(SaveGameInstance);
	SaveGameInstance->SaveLevelName(OpenLevelName);

	const auto PlayerReference = Cast<APlayerBase>(Player);
	ReturnIfNull(PlayerReference);
	SaveCommonPlayerData(SaveGameInstance, PlayerReference);
			
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySaveGameSlot"), 0);
	
	//Log message to let the player know the game is saved.
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Saved."));
			
#ifdef _DEBUG_GAMEINSTANCE
	UE_LOG(LogTemp, Warning, TEXT("Saved level is %s"), *SaveGameInstance->SavedLevel)
#endif
}

void UHookGameInstance::SaveCommonPlayerData(USavingAndLoading* SaveGameInstance, APlayerBase* PlayerRef)
{
	SaveGameInstance->PlayerLocation = PlayerRef->GetActorLocation();
	SaveGameInstance->SavedHealth = PlayerRef->CurrentHealth;
	SaveGameInstance->NPCFollowStatus = PlayerRef->bNPCFollowPlayer;
	SaveGameInstance->CameraSpringarmRotation = PlayerRef->SpringArm->GetRelativeRotation();
	
	SaveGameInstance->SkillTreeLevel = PlayerRef->SkillTreeFCRI->Level;
	SaveGameInstance->CurrentExperience = PlayerRef->SkillTreeFCRI->CurrentExperience;
	SaveGameInstance->SkillPoints = PlayerRef->SkillTreeFCRI->SkillPoints;
	SaveGameInstance->SwordKnockRank = PlayerRef->SkillTreeFCRI->SwordKnockBackUpgrade->CurrentRank;
	SaveGameInstance->SwordLengthRank = PlayerRef->SkillTreeFCRI->SwordLengthUpgrade->CurrentRank;
	SaveGameInstance->GrappleSpeedRank = PlayerRef->SkillTreeFCRI->GrappleSpeedUpgrade->CurrentRank;

	
}

void UHookGameInstance::LoadCommonPlayerData(USavingAndLoading* LoadGameInstance, APlayerBase* PlayerRef)
{
	PlayerRef->CurrentHealth = LoadGameInstance->SavedHealth;
	PlayerRef->SetActorLocation(LoadGameInstance->PlayerLocation);
	PlayerRef->bNPCFollowPlayer = LoadGameInstance->NPCFollowStatus;
	PlayerRef->SpringArm->SetWorldRotation(LoadGameInstance->CameraSpringarmRotation);
		  
	PlayerRef->SkillTreeFCRI->Level = LoadGameInstance->SkillTreeLevel;
	PlayerRef->SkillTreeFCRI->CurrentExperience = LoadGameInstance->CurrentExperience;
	PlayerRef->SkillTreeFCRI->SkillPoints = LoadGameInstance->SkillPoints;
	PlayerRef->SkillTreeFCRI->SwordKnockBackUpgrade->CurrentRank = LoadGameInstance->SwordKnockRank;
	PlayerRef->SkillTreeFCRI->SwordLengthUpgrade->CurrentRank = LoadGameInstance->SwordLengthRank;
	PlayerRef->SkillTreeFCRI->GrappleSpeedUpgrade->CurrentRank = LoadGameInstance->GrappleSpeedRank;
}

void UHookGameInstance::LoadGameData(AActor* Player)
{
	bool bDoesSaveGameExist = false;	
	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));
	
	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MySaveGameSlot", 0);
	ReturnIfNull(bDoesSaveGameExist);
	LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MySaveGameSlot", 0));
	ReturnIfNull(LoadGameSlot);

	UGameplayStatics::OpenLevel(GetWorld(), (TEXT("/Game/Maps/%s"), *LoadGameSlot->SavedLevel), TRAVEL_Absolute);
	bLevelIsLoaded = true;
	bNPCDataLoad = true;

	if (bLevelIsLoaded)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game map Loaded."));
	}
}

void UHookGameInstance::LoadPlayerData(APlayerBase* PlayerBase)
{	
	bool bDoesSaveGameExist = false;
	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MySaveGameSlot", 0);
	ReturnIfNull(bDoesSaveGameExist);
	LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MySaveGameSlot", 0));	
	ReturnIfNull(LoadGameSlot);

	LoadCommonPlayerData(LoadGameSlot, PlayerBase);
			
	#ifdef _DEBUG_GAMEINSTANCE
	FString PlayerLocation = LoadGameSlot->PlayerLocation.ToString();
	UE_LOG(LogTemp, Warning, TEXT("PlayerLocation saved is: %s"), *PlayerLocation);
	#endif
}

void UHookGameInstance::LoadNPCData(AAIRobotSidekick* Robot)
{
	bool bDoesSaveGameExist = false;
	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MySaveGameSlot", 0);
	ReturnIfNull(bDoesSaveGameExist);
	LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MySaveGameSlot", 0));
	ReturnIfNull(LoadGameSlot);
	ReturnIfNull(LoadGameSlot->NPCFollowStatus);

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

void UHookGameInstance::SaveLatestCheckpoint(AActor* Player)
{

	FString OpenLevel = GetWorld()->GetName();
	FName OpenLevelName = FName(*OpenLevel);

	const auto SaveGameInstance = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));
	ReturnIfNull(SaveGameInstance);
	SaveGameInstance->SaveLevelName(OpenLevelName);

	const auto PlayerReference = Cast<APlayerBase>(Player);
	ReturnIfNull(PlayerReference);
	SaveCommonPlayerData(SaveGameInstance, PlayerReference);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MyCheckpointSlot"), 1);
	
	//Log message to let the player know the checkpoint is reached and saved.
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Checkpoint reached."));*/

#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
	UE_LOG(LogTemp, Warning, TEXT("Checkpoint: Saved level is %s"), *SaveGameInstance->SavedLevel)
#endif
}

void UHookGameInstance::LoadLatestCheckpoint(AActor* Player)
{
	bool bDoesSaveGameExist = false;

	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));
	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MyCheckpointSlot", 1);
	ReturnIfNull(bDoesSaveGameExist);
	LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MyCheckpointSlot", 1));
	ReturnIfNull(LoadGameSlot);

	UGameplayStatics::OpenLevel(GetWorld(), (TEXT("/Game/Maps/%s"), *LoadGameSlot->SavedLevel), TRAVEL_Absolute);
	bCheckpointIsLoaded = true;
	bNPCCheckpointDataLoad = true;
	
	if (bCheckpointIsLoaded)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("bCheckpointIsLoaded."));
	}
}

void UHookGameInstance::LoadPlayerCheckpointData(APlayerBase* PlayerBase)
{
	bool bDoesSaveGameExist = false;

	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MyCheckpointSlot", 1);
	ReturnIfNull(bDoesSaveGameExist);
	
	LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MyCheckpointSlot", 1));
	ReturnIfNull(LoadGameSlot);
	
	LoadCommonPlayerData(LoadGameSlot, PlayerBase);

#ifdef _DEBUG_GAMEINSTANCE
	FString PlayerLocation = LoadGameSlot->PlayerLocation.ToString();
	UE_LOG(LogTemp, Warning, TEXT("PlayerLocation saved is: %s"), *PlayerLocation);
#endif
}

void UHookGameInstance::LoadNPCCheckpoint(AAIRobotSidekick* Robot)
{
	bool bDoesSaveGameExist = false;

	USavingAndLoading* LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::CreateSaveGameObject(USavingAndLoading::StaticClass()));

	bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist("MyCheckpointSlot", 1);
	ReturnIfNull(bDoesSaveGameExist);

	LoadGameSlot = Cast<USavingAndLoading>(UGameplayStatics::LoadGameFromSlot("MyCheckpointSlot", 1));
	ReturnIfNull(LoadGameSlot);
	ReturnIfNull(LoadGameSlot->NPCFollowStatus);
	
	FVector RobotSpawn = { 100.f,100.f,100.f };
	FVector PlayerLocation = LoadGameSlot->PlayerLocation;

	Robot->SetActorLocation(PlayerLocation + RobotSpawn);
	Robot->bInteractable = false;
	Robot->bFollowPlayer = true;
		
#ifdef _DEBUG_CHECKPOINTGAMEINSTANCE
	FString RobotLocation = (PlayerLocation + RobotSpawn).ToString();
	UE_LOG(LogTemp, Warning, TEXT("RobotLocation saved is: %s"), *RobotLocation);
#endif
}
