#include "AISquidbirdLeaperController.h"
#include "AISquidbirdLeaper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HookGame/Helpers/Macros.h"
#include "Kismet/GameplayStatics.h"

//#define _DEBUG_LEAPERCONTROLLER
void AAISquidbirdLeaperController::BeginPlay()
{
	Super::BeginPlay();
	SetStartValues();
}

void AAISquidbirdLeaperController::SetStartValues()
{
	ReturnIfNull(GetPawn<AAISquidbirdLeaper>());
	ReturnIfNull(AIBehavior);
#ifdef _DEBUG_LEAPERCONTROLLER
	UE_LOG(LogTemp, Warning, TEXT("Found AIBehavior!"));
#endif
	RunBehaviorTree(AIBehavior);
	ReturnIfNull(GetBlackboardComponent());
	
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	GetBlackboardComponent()->SetValueAsFloat(TEXT("LeapDistance"), GetPawn<AAISquidbirdLeaper>()->LeapDistance);
	GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), GetPawn<AAISquidbirdLeaper>()->bPlayerInRange);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("ResetTimer"), GetPawn<AAISquidbirdLeaper>()->ResetTimer);
}

void AAISquidbirdLeaperController::SetChangingValues()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ReturnIfNull(PlayerPawn);
	ReturnIfNull(GetPawn<AAISquidbirdLeaper>());

	GetBlackboardComponent()->SetValueAsVector(TEXT("LeapTarget"), GetPawn<AAISquidbirdLeaper>()->LeapTarget);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), GetPawn<AAIBehaviorBase>()->bIsDead);
	if (LineOfSightTo(PlayerPawn)) { // Checks if the AI Can see the player, and therefore if it can chase them.
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
	}
	else
	{
#ifdef _DEBUG_LEAPERCONTROLLER
		UE_LOG(LogTemp, Error, TEXT("Leaper can't see player!"));
#endif
	}
}

void AAISquidbirdLeaperController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetChangingValues();
}

