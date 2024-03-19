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
		UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Url from the config file: %s"), *BaseURL);
	}
	else
	{
		UE_LOG(LabyrAInthVR_Network_Log, Warning, TEXT("Could not take the Url from config file. Using default value: %s"), *BaseURL);	
	}
}

FString ANetworkController::GetLabyrinthFromBE(ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Preparing Request"));
	FHttpModule& HttpModule = FHttpModule::Get();

	FString LabyrinthURL = BaseURL + "/labyrinth";

	// Create an http request. The request will execute asynchronously, and call us back on the Lambda below
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = HttpModule.CreateRequest();

	// Set the http URL
	pRequest->SetURL(LabyrinthURL);

	// This is where we set the HTTP method (GET, POST, etc)
	pRequest->SetVerb(TEXT("POST"));	

	// Set the Header for a json content-type
	pRequest->SetHeader("Content-Type", TEXT("application/json"));

	const FString JsonString = SerializeLabyrinth(LabyrinthDTO);
	
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
				UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Connection SUCCESSFULL."));
				// We should have a JSON response - attempt to process it.
				// Validate http called us back on the Game Thread...
				FString ResponseContent = pResponse->GetContentAsString();
				if(DeSerializeLabyrinth(ResponseContent, LabyrinthDTO))
				{
					OnLabyrinthReceived.Broadcast();
					return "";
				}
				else
				{
					return "Could not deserialize Labyrinth received from the BackEnd";
				}
			} 
			else {
			   switch (pRequest->GetStatus()) {
			   case EHttpRequestStatus::Failed_ConnectionError:
			   	UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Connection failed."));
			   default:
			   	UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Request failed. %s"));
			   		}
			   }
		});
	UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Starting the request for the labyrinth."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}


FString ANetworkController::SerializeLabyrinth(ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LogTemp, Log, TEXT("Serializing Request"));
	// Creare un oggetto JSON per rappresentare LabyrinthDTO
	TSharedPtr<FJsonObject> LabyrinthDTOJson = MakeShareable(new FJsonObject);
	
	// Aggiungere il campo "level" all'oggetto JSON
	LabyrinthDTOJson->SetNumberField(TEXT("level"), LabyrinthDTO->Level);

	// Creare un array JSON per rappresentare LabyrinthStructure
	std::vector<std::vector<uint8>>& LabyrinthStructure = LabyrinthDTO->LabyrinthStructure;
	TArray<TSharedPtr<FJsonValue>> LabyrinthStructureArray;

	for (uint i = 0; i < LabyrinthStructure.size(); i++)
	{
		TArray<TSharedPtr<FJsonValue>> RowArray;
		for (uint j = 0; j < LabyrinthStructure[i].size(); j++)
		{
			RowArray.Add(MakeShareable(new FJsonValueNumber(LabyrinthStructure[i][j])));
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

bool ANetworkController::DeSerializeLabyrinth(FString LabyrinthString, ULabyrinthDTO* LabyrinthDto)
{
	check(IsInGameThread());

	TSharedPtr<FJsonObject> OutLabyrinth;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(LabyrinthString);
	if (!FJsonSerializer::Deserialize(JsonReader, OutLabyrinth))
	{
		return false;
	}
	// Estrai il campo "level" dall'oggetto "labyrinth"
	int32 Level;
	if (!OutLabyrinth->TryGetNumberField(TEXT("level"), LabyrinthDto->Level))
	{
		UE_LOG(LogTemp, Error, TEXT("Error during Level Deserialization: 'level' field not found or not a string."));
		return false;
	}

	// Estrai il campo "labyrinthStructure" dall'oggetto "labyrinth"
	const TArray<TSharedPtr<FJsonValue>>* LabyrinthStructureArray;
	if (!OutLabyrinth->TryGetArrayField(TEXT("labyrinthStructure"), LabyrinthStructureArray))
	{
		UE_LOG(LogTemp, Error, TEXT("Error during Labyrinth Structure Deserialization: 'labyrinthStructure' field not found or not an array."));
		return false;
	}

	// Estrai i valori dell'array e convertili in interi
	uint8 LabyrinthRows = LabyrinthStructureArray->Num();
	uint8 LabyrinthColumns = LabyrinthStructureArray->Top()->AsArray().Num();
	try
	{
		uint8 i = 0;
		LabyrinthDto->LabyrinthStructure.resize(LabyrinthRows, std::vector<uint8>(LabyrinthColumns, 0));
		for (auto LabyrinthRow : *LabyrinthStructureArray)
		{
			uint8 j = 0;
			for (auto Value : LabyrinthRow->AsArray())
			{
				uint8 LabValue;
				if (Value.IsValid() && Value->Type == EJson::Number && Value->TryGetNumber(LabValue))
				{
					LabyrinthDto->LabyrinthStructure[i][j] = LabValue;
				}
				j++;
			}
			i++;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}
