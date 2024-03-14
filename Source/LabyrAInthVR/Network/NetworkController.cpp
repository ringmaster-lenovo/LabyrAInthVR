// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkController.h"
#include "HttpModule.h"
#include "LabyrinthDTO.h"
#include "Interfaces/IHttpResponse.h"


// Sets default values
ANetworkController::ANetworkController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANetworkController::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the config directory path
	FString ConfigDir = FPaths::ProjectConfigDir();

	// Get the complete path to the NetworkController.ini
	FString NetworkConfigFilePath = FPaths::Combine(*ConfigDir, TEXT("NetworkController.ini"));
	
	NetworkConfigFilePath = FConfigCacheIni::NormalizeConfigIniPath(NetworkConfigFilePath);

	if(GConfig->GetString(TEXT("BackendSettings"), TEXT("BaseUrl"), BaseURL, NetworkConfigFilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Url from the config file: %s"), *BaseURL);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not take the Url from config file. Using default value: %s"), *BaseURL);	
	}
	
}

bool ANetworkController::GetLabyrinthFromGPT(ULabyrinthDTO* LabyrinthDTO)
{
	FHttpModule& HttpModule = FHttpModule::Get();

	FString LabyrinthEndPoint = "/labyrinth";

	FString LabyrinthURL = BaseURL + LabyrinthEndPoint;

	// Create an http request
	// The request will execute asynchronously, and call us back on the Lambda below
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = HttpModule.CreateRequest();

	// Set the http URL
	pRequest->SetURL(LabyrinthURL);

	// This is where we set the HTTP method (GET, POST, etc)
	pRequest->SetVerb(TEXT("POST"));	

	// Eventually set the Header and the RequestContent
	pRequest->SetHeader("Content-Type", TEXT("application/json"));

	FString JsonString = SerializeLabyrinth(LabyrinthDTO);
	
	pRequest->SetContentAsString(JsonString);

	// Set the callback, which will execute when the HTTP call is complete
	pRequest->OnProcessRequestComplete().BindLambda(
		// Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
		// class's methods in the callback.
		[&](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully) {
				UE_LOG(LogTemp, Log, TEXT("Connection SUCCESSFULL."));
				// We should have a JSON response - attempt to process it.
				// Validate http called us back on the Game Thread...
				FString ResponseContent = pResponse->GetContentAsString();	
			} 
			else {
			   switch (pRequest->GetStatus()) {
			   case EHttpRequestStatus::Failed_ConnectionError:
			   	UE_LOG(LogTemp, Error, TEXT("Connection failed."));
			   default:
			   	UE_LOG(LogTemp, Error, TEXT("Request failed. %s"));
			   	}
			   }
		});
	UE_LOG(LogTemp, Log, TEXT("Starting the request for the maze."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
	}


FString ANetworkController::SerializeLabyrinth(ULabyrinthDTO* LabyrinthDTO)
{
	// Creare un oggetto JSON per rappresentare LabyrinthDTO
	TSharedPtr<FJsonObject> LabyrinthDTOJson = MakeShareable(new FJsonObject);
	
	// Aggiungere il campo "level" all'oggetto JSON
	LabyrinthDTOJson->SetNumberField(TEXT("level"), LabyrinthDTO->Level);

	// Creare un array JSON per rappresentare LabyrinthStructure
	std::vector<std::vector<uint8>>& LabyrinthStructure = LabyrinthDTO->LabyrinthStructure;
	TArray<TSharedPtr<FJsonValue>> LabyrinthStructureArray;
	for (const auto& Row : LabyrinthStructure)
	{
		TArray<TSharedPtr<FJsonValue>> RowArray;
		for (const auto& Value : Row)
		{
			RowArray.Add(MakeShareable(new FJsonValueNumber(Value)));
		}
		LabyrinthStructureArray.Add(MakeShareable(new FJsonValueArray(RowArray)));
	}
	
	// Aggiungere il campo "labyrinthStructure" all'oggetto JSON
	LabyrinthDTOJson->SetArrayField(TEXT("labyrinthStructure"), LabyrinthStructureArray);
	
	// Serializzare l'oggetto JSON in una stringa
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(LabyrinthDTOJson.ToSharedRef(), JsonWriter);
	return JsonString;
}

ULabyrinthDTO ANetworkController::DeSerializeLabyrinth(FString LabyrinthString)
{
	check(IsInGameThread());
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(LabyrintString);
	TSharedPtr<FJsonObject> OutLabyrinth;

	if (FJsonSerializer::Deserialize(JsonReader, OutLabyrinth))
	{
		int32 Level;
		TArray<TArray<int32>> LabyrinthStructure;

		// Serializza l'oggetto JSON in una stringa
		FString ResponseJsonString;
		TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ResponseJsonString);
		FJsonSerializer::Serialize(OutLabyrinth.ToSharedRef(), JsonWriter);

		// Estrai il campo "level" dall'oggetto "labyrinth"
		if (!OutLabyrinth->TryGetNumberField(TEXT("level"), Level))
		{
			UE_LOG(LogTemp, Error, TEXT("Error during Level Deserialization: 'level' field not found or not a string."));
			return;
		}

		// Estrai il campo "labyrinthStructure" dall'oggetto "labyrinth"
		const TArray<TSharedPtr<FJsonValue>>* LabyrinthStructureArray;
		if (!OutLabyrinth->TryGetArrayField(TEXT("labyrinthStructure"), LabyrinthStructureArray))
		{
			UE_LOG(LogTemp, Error, TEXT("Error during Labyrinth Structure Deserialization: 'labyrinthStructure' field not found or not an array."));
			return;
		}

		// Estrai i valori dell'array e convertili in interi
		for (const auto& LabyrinthRow : *LabyrinthStructureArray)
		{
			TArray<int32> Row;
			const TArray<TSharedPtr<FJsonValue>>& RowArray = LabyrinthRow->AsArray();
			for (const auto& Value : RowArray)
			{
				if (Value.IsValid() && Value->Type == EJson::Number)
				{
					Row.Add(Value->AsNumber());
				}
			}
			LabyrinthStructure.Add(Row);
		}


		// Imposta i dati della struttura LabyrinthDTO
		LabyrinthDTO.Level = Level;
		LabyrinthDTO.LabyrinthStructure = LabyrinthStructure;
		}
		return LabyrinthDTO;
		
}
