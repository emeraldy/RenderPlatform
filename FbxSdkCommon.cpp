//-----------------------------------------------------------------
// Perform initialisation, scene loading and destruction of sdk
// objects that are common to all fbx importers.
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "FbxSdkCommon.h"

using namespace Emerald;

void FbxSdkCommon::InitializeSdkObjects()
{
    pManager = FbxManager::Create();
    if (!pManager)
    {
        MessageBox(NULL, TEXT("Failed to create fbx manager."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    pScene = FbxScene::Create(pManager, "Asset Scene");
    if (!pScene)
    {
        MessageBox(NULL, TEXT("Failed to create fbx scene."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
}

void FbxSdkCommon::DestroySdkObjects()
{
    if (pManager)
    {
        pManager->Destroy();
    }
}

bool FbxSdkCommon::LoadScene(const char* pFileName)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor, lSDKMinor, lSDKRevision;
    int lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    FbxImporter* lImporter = FbxImporter::Create(pManager, "");

    const bool lImportStatus = lImporter->Initialize(pFileName, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if (!lImportStatus)
    {
        FbxString error("Call to FbxImporter::Initialize() failed. ");
        error += "Error returned: ";
        error += lImporter->GetStatus().GetErrorString();
        std::wstring errorInfo = ConvertCharStringToWide(std::string(error.Buffer()));
        MessageBox(NULL, errorInfo.c_str(), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            error = "FBX file format version for this FBX SDK is ";
            error += lSDKMajor;
            error += ".";
            error += lSDKMinor;
            error += ".";
            error += lSDKRevision;
            error += " but FBX file format version for file ";
            error += pFileName;
            error += "is";
            error += lFileMajor;
            error += ".";
            error += lFileMinor; 
            error += ".";
            error += lFileRevision;
            
            errorInfo = ConvertCharStringToWide(std::string(error.Buffer()));
            MessageBox(NULL, errorInfo.c_str(), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }

        return false;
    }

    FbxIOSettings* pIOSettings = pManager->GetIOSettings();
    if (lImporter->IsFBX())
    {
        // Set the import states
        pIOSettings->SetBoolProp(IMP_FBX_MATERIAL, true);
        pIOSettings->SetBoolProp(IMP_FBX_TEXTURE, true);
        pIOSettings->SetBoolProp(IMP_FBX_LINK, true);
        pIOSettings->SetBoolProp(IMP_FBX_SHAPE, true);
        pIOSettings->SetBoolProp(IMP_FBX_GOBO, true);
        pIOSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
        pIOSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);
    if (!lStatus || (lImporter->GetStatus() != FbxStatus::eSuccess))
    {
        if (lStatus)
        {
            FbxString error("The importer was able to read the file but with errors. Loaded scene may be incomplete.");
            std::wstring errorInfo = ConvertCharStringToWide(std::string(error.Buffer()));
            MessageBox(NULL, errorInfo.c_str(), TEXT("WARNING"), MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {
            FbxString error("Importer failed to load the file!");
            std::wstring errorInfo = ConvertCharStringToWide(std::string(error.Buffer()));
            MessageBox(NULL, errorInfo.c_str(), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}