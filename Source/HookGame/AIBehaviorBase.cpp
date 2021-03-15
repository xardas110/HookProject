// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBehaviorBase.h"
#include "AIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBase.h"


// Sets default values
AAIBehaviorBase::AAIBehaviorBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AAIBehaviorBase::OnAttach()
{
	UE_LOG(LogTemp, Warning, TEXT("enemy has been grappled!"));
	if (Controller)

	bIsGrabbed = true;
}

void AAIBehaviorBase::OnDetach()
{
	UE_LOG(LogTemp, Warning, TEXT("enemy has been released!"));
	bIsGrabbed = false;
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetMesh()->SetSimulatePhysics(false);
}

void AAIBehaviorBase::OnBeginOverlapAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Initiated"));
}

void AAIBehaviorBase::OnEndOverlapAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Ended"));
}

void AAIBehaviorBase::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Initiated, Bool change"));
}

void AAIBehaviorBase::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Default Attack Ended, bool change"));
}

// Called when the game starts or when spawned
void AAIBehaviorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAIBehaviorBase::Dodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Default Dodge called"));
	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	
}

// Called every frame
void AAIBehaviorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSinceAttack += DeltaTime;
	APawn* PlayerPawn=UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(PlayerPawn)
	{
		APlayerBase* Player = Cast<APlayerBase>(PlayerPawn);
		if(Player)
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
	}
}

// Called to bind functionality to input
void AAIBehaviorBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAIBehaviorBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const auto IncomingDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HealthTotal -= FMath::Min(HealthTotal, IncomingDamage);
	if (HealthTotal <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No health remaining"));
		bIsDead=true;
	}
	return IncomingDamage;
}

