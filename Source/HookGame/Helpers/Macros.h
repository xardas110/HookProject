#pragma once

bool inline PrintLine(const char* str)
{
	const FString FStr(str);
	UE_LOG(LogTemp, Warning, TEXT("%s returned nullptr"), *FStr);
	return false;
}

void inline Log(const char* str)
{
	const FString FStr(str);
	UE_LOG(LogTemp, Error, TEXT("%s returned nullptr"), *FStr);
}

#define ReturnIfNull(name) if (!name) if (!PrintLine(#name)) return
#define LogIfFailed(Expression) if (!(Expression)) Log(#Expression)

#define CreateAndLogIfFailed(Ptr, Type) \
	Ptr = CreateDefaultSubobject<Type>(TEXT(#Ptr)); \
	if (!Ptr) Log(#Ptr)

#define LogError(Text, ...) UE_LOG(LogTemp, Error, TEXT(Text), __VA_ARGS__)

#define LogWarning(Text, ...) UE_LOG(LogTemp, Warning, TEXT(Text), __VA_ARGS__)