// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBehaviorBase.h"
#include "AIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBase.h"
#include "PlayerSword.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIRobotSidekick.h"

//#define _DEBUG_AIBEHAVIOR

AAIBehaviorBase::AAIBehaviorBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAIBehaviorBase::UpdateStunDuration(const float DeltaTime)
{
	if (bCanAttack)
		return;
	
	CurrentStunDuration += DeltaTime;
	if (CurrentStunDuration >= StunDurationAfterGrabbed)
		bCanAttack = true;
	
}

void AAIBehaviorBase::BeginPlay()
{
	Super::BeginPlay();
	
	StandingZValue = GetActorLocation().Z;
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

void AAIBehaviorBase::OnBeginOverlapAttack()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Initiated"));
#endif
}

void AAIBehaviorBase::OnEndOverlapAttack()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Ended"));
#endif
}

void AAIBehaviorBase::Attack()
{	
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Initiated, Bool change"));
#endif
}

void AAIBehaviorBase::EndAttack()
{
	DamagedActors.Empty();
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Ended, bool change"));
#endif
}

void AAIBehaviorBase::HandleDeath()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Death handle called"));
#endif
}

void AAIBehaviorBase::FollowPlayer()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Follow Player called"));
#endif
}

void AAIBehaviorBase::Dodge()
{
#ifdef _DEBUG_AIBEHAVIOR
	UE_LOG(LogTemp, Warning, TEXT("Default Dodge called"));
#endif
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	
}

// Called every frame
void AAIBehaviorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSinceAttack += DeltaTime;
	UpdateStunDuration(DeltaTime);
	
	APawn* PlayerPawn=UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TimeSinceReleased += DeltaTime;
	if(PlayerPawn)
	{
		APlayerBase* Player = Cast<APlayerBase>(PlayerPawn);
		if(Player)
		{
			FVector PlayerPosition = Player->GetActorLocation();
			FVector MyPosition = GetActorLocation();
			float PlayerLastSeen = 0.f;
			
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
			if (FVector::Dist(MyPosition, PlayerPosition) < DetectionRange) {
				bPlayerInRange = true;
				bReturnToStart = false;
				PlayerLastSeen = GetWorld()->GetTimeSeconds();
			}
			else {
				bPlayerInRange = false;
			}

			if (GetWorld()->GetTimeSeconds() > PlayerLastSeen + ResetTimer) {
				bReturnToStart = true;
			}
		}
	}
	
	if(bIsDead)
	{
		TimeSinceDeath += DeltaTime;
		if(TimeSinceDeath>=TimeUntilDespawn)
		{
			Destroy();
		}
	}
	
	/*if(TimeSinceReleased>RecoveryTime&&!bIsStanding)
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetSimulatePhysics(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}*/
	
	if(bKnockedBack&&TimeSinceReleased>RecoveryTime)
	{
		bKnockedBack = false;
	}
}

// Called to bind functionality to input
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
	UWorld* GameWorld = GetWorld();
	if (GameWorld) {
		if (DamageCauser->IsA(APlayerSword::StaticClass())) {
			bIsAttacking = false;
			float KnockbackValue = Cast<APlayerSword>(DamageCauser)->KnockBackValue;
			const auto PlayerController = GameWorld->GetFirstPlayerController();
			if (PlayerController)
			{
				bKnockedBack = true;
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
		}
	}
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


