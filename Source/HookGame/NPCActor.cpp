// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCActor.h"
#include "PlayerBase.h"
#include "Components/AudioComponent.h"

// Sets default values
//ANPCActor::ANPCActor()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	//PrimaryActorTick.bCanEverTick = true;
//
//	//RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
//
//	//NPCMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NPC Mesh"));
//	//NPCMesh->SetupAttachment(RootComponent);
//
//	//NPCMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NPC Mesh 2"));
//	//NPCMesh2->SetupAttachment(NPCMesh);
//
//	//InteractionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
//	//InteractionComp->SetupAttachment(NPCMesh);
//
//	
//
//}
//
//// Called when the game starts or when spawned
//void ANPCActor::BeginPlay()
//{
//	Super::BeginPlay();
//
//	//InteractionComp->OnComponentBeginOverlap.AddDynamic(this, &ANPCActor::InteractionActive);
//	//InteractionComp->OnComponentEndOverlap.AddDynamic(this, &ANPCActor::InteractionNotActive);
//	
//}
//
//void ANPCActor::Talk(USoundBase* SFX, TArray<FSubtitle> Subs)
//{
//	/*APlayerBase* Char = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));*/
//
//
//	//***Uncomment this when/if we have audio***
//	//
//	//AudioComp->SetSound(SFX);
//	//AudioComp->Play();
//
//	//Tell the UI to update with the new subtitles
//	//Char->GetUI()->UpdateSubtitles(Subs);
//}
//
//// Called every frame
//void ANPCActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//void ANPCActor::InteractionActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	//if(OtherActor->IsA<APlayerBase>())
//	//{
//	//	//UE_LOG(LogTemp, Warning, TEXT("NPC registered that you are in the interaction area"));
//	//	//APlayerBase* Char = Cast<APlayerBase>(OtherActor);
//	//	//Char->SetTalkRangeStatus(true);
//	//	//Char->GeneratePlayerLines(PlayerLines);
//	//	//Char->SetAssociatedPawn(this);
//	//	//Char->SetInteractableNPC(this);
//	//	//Char->DisplayInteractMessage();
//	//	
//	//}
//
//}
//
//void ANPCActor::InteractionNotActive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
//{
//	//UE_LOG(LogTemp, Warning, TEXT("NPC registered that you are no longer in the interaction area"));
//	//if (OtherActor->IsA<APlayerBase>())
//	{
//		//APlayerBase* Char = Cast<APlayerBase>(OtherActor);
//		//Char->SetTalkRangeStatus(false);
//		//Char->SetAssociatedPawn(nullptr);
//		//Char->SetInteractableNPC(nullptr);
//		
//		//if(bCheckNPCFollowStatus)
//		//{
//
//		//	UE_LOG(LogTemp, Warning, TEXT("Follow status is true"));
//		//	
//		//	//UWorld* GameWorld = GetWorld();
//
//		//	//if(GameWorld && SpawnableNPC)
//		//	//{
//		//	//	//GameWorld->SpawnActor<AAIRobotSidekick>(SpawnableNPC, GetActorLocation(), GetActorRotation());
//		//	//	//bSpawnedNPC = true;
//
//		//	//}
//		//}
//		
//	}
//}
//
//
//void ANPCActor::AnswerPlayerCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay)
//{
//	//if (!NPCLines) return;
//
//	//FString ContextString;
//	//FDialogue* Dialog = NPCLines->FindRow<FDialogue>(PlayerLine, ContextString);
//
//	//APlayerBase* MainChar = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//
//	//if (Dialog && MainChar)
//	//{
//	//	FTimerHandle TimerHandle;
//	//	FTimerDelegate TimerDel;
//
//	//	TimerDel.BindUFunction(this, FName("Talk"), Dialog->DialogueSoundEffect, Dialog->Subtitles);
//
//	//	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);
//	//}
//}
