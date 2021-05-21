// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBehaviorBase.h"
#include "AIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/PlayerBase.h"
#include "../Character/Actors/PlayerSword.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Helpers/Macros.h"

//#define _DEBUG_AIBEHAVIOR

AAIBehaviorBase::AAIBehaviorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GrappleTracer = CreateDefaultSubobject<UGrappleTracer>(TEXT("Tracer"));
}

void AAIBehaviorBase::BeginPlay()
{
	Super::BeginPlay();
	GrappleTracer->ComponentExtent->SetBoxExtent({ 75.f, 75.f, 150.f });
	StandingZValue = GetActorLocation().Z;
}

void AAIBehaviorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GrappleTracer->GrapplePoint->SetWorldLocation(GetActorLocation());
	GrappleTracer->ComponentExtent->SetWorldLocation(GetActorLocation());
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	UpdateStunDuration(DeltaTime);
	UpdateAi(PlayerPawn, DeltaTime);
	StunRecoveryTimer(DeltaTime);
	if(bIsDead)
	{
		DeathDespawnTimer(DeltaTime);
	}	
	
}

void AAIBehaviorBase::UpdateAi(_In_ APawn* PlayerPawn, const float DeltaTime)
{
	ReturnIfNull(PlayerPawn);
	APlayerBase* Player = Cast<APlayerBase>(PlayerPawn);
	ReturnIfNull(Player);

	const FVector PlayerPosition = Player->GetActorLocation();
	const FVector MyPosition = GetActorLocation();
	float PlayerLastSeen = 0.f;

	UpdateDodge(Player, DeltaTime);	
	CheckIfPlayerIsInRange(MyPosition, PlayerPosition, PlayerLastSeen, DeltaTime);

	if (GetWorld()->GetTimeSeconds() > PlayerLastSeen + ResetTimer) {
		bReturnToStart = true;
	}
}

void AAIBehaviorBase::UpdateDodge(_Inout_ class APlayerBase* Player, const float DeltaTime)
{
	if (TimeSinceAttack >= DodgeCooldown)
	{
		if (Player->_AttackState != AttackState::Default)
		{
			if (FMath::RandRange(1, 100 / DodgeChance) == 1)
			{
				Dodge();
				TimeSinceAttack = 0.f;
			}
		}
	}
}

void AAIBehaviorBase::CheckIfPlayerIsInRange(const _In_ FVector& MyLocation, const _In_ FVector& PlayerLocation, _Inout_ float& PlayerLastSeen, const float DeltaTime)
{
	if (FVector::Dist(MyLocation, PlayerLocation) < DetectionRange) {
		bPlayerInRange = true;
		bReturnToStart = false;
		PlayerLastSeen = GetWorld()->GetTimeSeconds();
	}
	else {
		if (!bFollowLinkedAiCommand)
			bPlayerInRange = false;
	}
}

void AAIBehaviorBase::UpdateStunDuration(const float DeltaTime)
{
	if (bCanAttack)
		return;
	
	CurrentStunDuration += DeltaTime;
	if (CurrentStunDuration >= StunDurationAfterGrabbed)
		bCanAttack = true;
	
}

void AAIBehaviorBase::OnAttach()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("enemy has been grappled!"));
#endif
	bIsGrabbed = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAIBehaviorBase::OnDetach()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("enemy has been released!"));
#endif
	bIsGrabbed = false;
	bCanAttack = false;
	CurrentStunDuration = 0.f;
	
	SetActorRotation(FRotator::ZeroRotator);
	AddActorWorldOffset({ 0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()});
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAIBehaviorBase::OnPlayerSpotted(const _In_ AAIBehaviorBase* Parent, const _In_ FVector& PlayerLocation)
{
	const auto AIController = Cast<AAIController>(GetController());
	ReturnIfNull(AIController);
	const auto BlackBoard = AIController->GetBlackboardComponent();
	ReturnIfNull(BlackBoard);
	ReturnIfNull(Parent);

	if (Parent->bPlayerInRange)
	{
		BlackBoard->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
		bPlayerInRange = true;
		bFollowLinkedAiCommand = true;
	}

}

void AAIBehaviorBase::Dodge()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Dodge called"));
#endif
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);	
}

void AAIBehaviorBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AAIBehaviorBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const auto IncomingDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageKnockback(DamageCauser);
	HealthTotal -= FMath::Min(HealthTotal, IncomingDamage);
	
	if (HealthTotal <= 0) {
#ifdef _DEBUG_AIBEHAVIOR
		UE_LOG(LogTemp, Warning, TEXT("No health remaining"));
#endif
		bIsDead = true;
		HandleDeath();
	}
	return IncomingDamage;
}

void AAIBehaviorBase::DamageKnockback(AActor* DamageCauser)
{
	if (bKnockbackImmune)
		return;
	UWorld* GameWorld = GetWorld();
	ReturnIfNull(GameWorld);
	ReturnIfNull(DamageCauser->IsA(APlayerSword::StaticClass()));
	const auto PlayerController = GameWorld->GetFirstPlayerController();
	ReturnIfNull(PlayerController);
	
	bIsAttacking = false;
	bKnockedBack = true;
	bKnockbackImmune = true;
	
	float KnockbackValue = Cast<APlayerSword>(DamageCauser)->KnockBackValue;
	TimeSinceReleased = 0.f;
	FVector KnockbackVector = PlayerController->GetPawn()->GetActorLocation();
	FVector Direction = KnockbackVector - GetActorLocation();
	Direction /= Direction.Size();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->Velocity -= Direction * (KnockbackValue*WeightModifier());
#ifdef _DEBUG_AIBEHAVIOR
				UE_LOG(LogTemp, Error, TEXT("KNOCKBACK!"));
#endif
}

float AAIBehaviorBase::WeightModifier()
{
	static float MyWeight = GetCapsuleComponent()->UPrimitiveComponent::GetMass();
	if (MyWeight > 100.f)
		return 0.5f;
	if (MyWeight < 100.f)
		return 3.f;
	if (MyWeight == 100.f)
		return 1.f;
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" Could not get weight for knockback! "));
		return 1.f;
	}
}

void AAIBehaviorBase::StunRecoveryTimer(float DeltaTime)
{
	TimeSinceAttack += DeltaTime;
	TimeSinceReleased += DeltaTime;
	TimeSinceKnockedBack += DeltaTime;
	
	if (bKnockedBack && TimeSinceReleased > RecoveryTime)
	{
		bKnockedBack = false;
		TimeSinceKnockedBack = 0.f;
	}
	if (TimeSinceKnockedBack>KnockbackImmunityTime)
	{
		bKnockbackImmune = false;
	}
}

void AAIBehaviorBase::DeathDespawnTimer(float DeltaSeconds)
{
	TimeSinceDeath += DeltaSeconds;
	if (TimeSinceDeath >= TimeUntilDespawn)
	{
		Destroy();
	}
}

// Functions to be overridden in inherited classes below //

void AAIBehaviorBase::HandleDeath()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Death handle called"));
#endif
}

void AAIBehaviorBase::EndAttack()
{
	DamagedActors.Empty();
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Ended, bool change"));
#endif
}

void AAIBehaviorBase::Attack()
{	
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Initiated, Bool change"));
#endif
}

void AAIBehaviorBase::OnEndOverlapAttack()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Ended"));
#endif
}

void AAIBehaviorBase::OnBeginOverlapAttack()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Initiated"));
#endif
}

void AAIBehaviorBase::FollowPlayer()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Follow Player called"));
#endif
}