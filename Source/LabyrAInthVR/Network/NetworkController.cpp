// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkController.h"
#include "HttpModule.h"
#include "DTO/LabyrinthDTO.h"
#include "Interfaces/IHttpResponse.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Network_Log);

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

	if (GConfig->GetString(TEXT("BackendSettings"), TEXT("BaseUrl"), BaseURL, NetworkConfigFilePath))
	{
		UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Url from the config file: %s"), *BaseURL);
	}
	else
	{
		UE_LOG(LabyrAInthVR_Network_Log, Warning, TEXT("Could not take the Url from config file. Using default value: %s"), *BaseURL);	
	}
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> ANetworkController::GetRequest(FString Url, FString Method, FString* ObjectDTO)
{
	FHttpModule& HttpModule = FHttpModule::Get();
	// Create an http request. The request will execute asynchronously, and call us back on the Lambda below
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = HttpModule.CreateRequest();

	// Set the http URL
	pRequest->SetURL(Url);
	
	// This is where we set the HTTP method (GET, POST, etc)
	if ((Method != "POST") || (Method != "GET"))
	{
		pRequest->SetVerb(TEXT("POST"));	
	}
	else
	{
		pRequest->SetVerb(Method);
	}
	if ((Method == "POST") && (ObjectDTO != nullptr))
	{
		pRequest->SetContentAsString(*ObjectDTO);
	}

	// Set the Header for a json content-type
	pRequest->SetHeader("Content-Type", TEXT("application/json"));

}

void ANetworkController::GetLabyrinthFromBE(ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Preparing Request for input labyrinth"));
	const FString JsonString = SerializeLabyrinth(LabyrinthDTO);

	FString Method = "POST";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = GetRequest(LabyrinthURL, Method, JsonString);
	
	// Set the callback, which will execute when the HTTP call is complete
	pRequest->OnProcessRequestComplete().BindLambda(
		// Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
		// class's methods in the callback.
		[this, LabyrinthDTO](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully)
			{
				UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Connection SUCCESSFULL."));
				// We should have a JSON response - attempt to process it.
				// Validate http called us back on the Game Thread...
				FString ResponseContent = pResponse->GetContentAsString();
				if (DeSerializeLabyrinth(ResponseContent, LabyrinthDTO))
				{
					OnLabyrinthReceived.Broadcast();
				}
				else
				{
					OnNetworkError.Broadcast();
					UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Cannot Deserialize Json Response"));
				}
			} 
			else {
			   switch (pRequest->GetStatus()) {
					case EHttpRequestStatus::Failed_ConnectionError:
						{
			   				UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Connection failed."));
							break;
						}
					default:
						{
							UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Request failed. "));
							break;
						}
			   		}
			   }
				OnNetworkError.Broadcast();
		});
	UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Starting the request for the labyrinth."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}


FString ANetworkController::SerializeLabyrinth(ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Serializing Request for labyrinth"));
	// Create a Json object to hold the LabyrinthDTO
	TSharedPtr<FJsonObject> LabyrinthDTOJson = MakeShareable(new FJsonObject);
	
	// Set the level field
	LabyrinthDTOJson->SetNumberField(TEXT("level"), LabyrinthDTO->Level);

	// Create a 2D array to hold the labyrinth structure
	std::vector<std::vector<uint8>>& LabyrinthStructure = LabyrinthDTO->LabyrinthStructure;
	TArray<TSharedPtr<FJsonValue>> LabyrinthStructureArray;

	for (int i = 0; i < LabyrinthStructure.size(); i++)
	{
		TArray<TSharedPtr<FJsonValue>> RowArray;
		for (int j = 0; j < LabyrinthStructure[i].size(); j++)
		{
			RowArray.Add(MakeShareable(new FJsonValueNumber(LabyrinthStructure[i][j])));
		}
		LabyrinthStructureArray.Add(MakeShareable(new FJsonValueArray(RowArray)));
	}
	
	// Add the level field to the Json object
	LabyrinthDTOJson->SetArrayField(TEXT("labyrinthStructure"), LabyrinthStructureArray);
	
	// Serialize the Json object to a string
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(LabyrinthDTOJson.ToSharedRef(), JsonWriter);
	return JsonString;
}

bool ANetworkController::DeSerializeLabyrinth(FString LabyrinthString, ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Deserializing labyrinth"));
	check(IsInGameThread());

	if (LabyrinthDTO == nullptr)
	{
		return false;
	}

	TSharedPtr<FJsonObject> OutLabyrinth;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(LabyrinthString);
	if (!FJsonSerializer::Deserialize(JsonReader, OutLabyrinth))
	{
		return false;
	}
	
	// Get the level field from the Json object
	if (!OutLabyrinth->TryGetNumberField(TEXT("level"), LabyrinthDTO->Level))
	{
		UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Error during Level Deserialization: 'level' field not found or not a string."));
		return false;
	}

	// Get the labyrinthStructure field from the Json object
	const TArray<TSharedPtr<FJsonValue>>* LabyrinthStructureArray;
	if (!OutLabyrinth->TryGetArrayField(TEXT("labyrinthStructure"), LabyrinthStructureArray))
	{
		UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Error during Labyrinth Structure Deserialization: 'labyrinthStructure' field not found or not an array."));
		return false;
	}

	try
	{
		// Extract the values from the Json array and store them in the LabyrinthDTO
		int32 LabyrinthRows = LabyrinthStructureArray->Num();
		int32 LabyrinthColumns = (LabyrinthRows > 0) ? (*LabyrinthStructureArray)[0]->AsArray().Num() : 0;
		LabyrinthDTO->LabyrinthStructure.resize(LabyrinthRows, std::vector<uint8>(LabyrinthColumns));
		
		uint8 i = 0;
		for (auto LabyrinthRow : *LabyrinthStructureArray)
		{
			uint8 j = 0;
			for (auto Value : LabyrinthRow->AsArray())
			{
				uint8 LabValue;
				if (Value.IsValid() && Value->Type == EJson::Number && Value->TryGetNumber(LabValue))
				{
					LabyrinthDTO->LabyrinthStructure[i][j] = LabValue;
				}
				j++;
			}
			i++;
		}
	}
	catch (...)
	{
		FString ErrorMessage = TEXT("Si è verificato un errore durante il ridimensionamento della matrice del labirinto.");
		UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("%s"), *ErrorMessage);
		return false;
	}

	for (int i = 0; i < std::size(LabyrinthDTO->LabyrinthStructure); i++)
	{
		for (int j = 0; j < std::size(LabyrinthDTO->LabyrinthStructure[0]); j++)
		{
			UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("%d "), LabyrinthDTO->LabyrinthStructure[i][j]);
		}
		UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("\n "));
	}
	return true;
}

void ANetworkController::FinishGame(UFinishGameRequestDTO FinishGameRequestDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Preparing Request for FinishGameRequest"));
	
	TSharedPtr<FJsonObject> FinishGameDTOJson = MakeShareable(new FJsonObject);
	FinishGameDTOJson->SetStringField("username", FinishGameRequestDTO.username);
	FinishGameDTOJson->SetNumberField("score", FinishGameRequestDTO.score);
	
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(FinishGameDTOJson.ToSharedRef(), JsonWriter);

	FString Method = "POST";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = GetRequest(LeaderboardUrl, Method, JsonString);
	
	pRequest->OnProcessRequestComplete().BindLambda(
		[this](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully)
			{
				UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("FinishGameDTO sent succesfully."));
			} 
			else {
			   switch (pRequest->GetStatus()) {
					case EHttpRequestStatus::Failed_ConnectionError:
						{
			   				UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Connection failed."));
							break;
						}
					default:
						{
							UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("FinishGameRequest failed. "));
							break;
						}
			   		}
			   }
				OnNetworkError.Broadcast();
		});
	UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Starting the request for the FinishGame."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}

TArray<ULeaderBoardDTO> ANetworkController::GetAllLeaderboards()
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Preparing Request for GetAllLeaderboards"));

	FString Method = "GET";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = GetRequest(LeaderboardUrl, Method, nullptr);
	
	pRequest->OnProcessRequestComplete().BindLambda(
		[this](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully)
			{
				if (DeserializeAllLEaderBoards(pResponse->GetContentAsString()))
				{
					
					UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("All Leaderboards retrieved succesfully."));
				}
				else
				{
					
				}
			} 
			else {
			   switch (pRequest->GetStatus()) {
					case EHttpRequestStatus::Failed_ConnectionError:
						{
							UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Connection failed."));
							break;
						}
					default:
						{
							UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("FinishGameRequest failed. "));
							break;
						}
					   }
			   }
				OnNetworkError.Broadcast();
		});
	UE_LOG(LabyrAInthVR_Network_Log, Log, TEXT("Starting the request for the GetAllLeaderBoards."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}