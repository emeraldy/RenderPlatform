//-----------------------------------------------------------------
// Resource Manager class
// C++ Source - ResourceManager.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ResourceManager.h"

//-----------------------------------------------------------------
// helper function definitions
//-----------------------------------------------------------------
//callback function for libpng reading from memory
void LoadPNGFromMemory(png_structp png_ptr, png_bytep data, png_size_t length);

//-----------------------------------------------------------------
// Class function definitions
//-----------------------------------------------------------------
ResourceManager::ResourceManager()
{
    //text resource pool initialisation
    m_textResourceCount = 2;
    m_ppTextResources = new WCHAR * [m_textResourceCount];
    for (int i = 0; i < m_textResourceCount; i++)
    {
        m_ppTextResources[i] = NULL;
    }
    m_currentTextResourceIndex = 0;

    //GLSL effect pool initialisation
    m_GLSLEffectResourceCount = 1;
    m_pGLSLEffectResources = new GLSLEffect[m_GLSLEffectResourceCount];
    for (int i = 0; i < m_GLSLEffectResourceCount; i++)
    {
        m_pGLSLEffectResources[i].pGLSLShaders = NULL;//always initialise a pointer to null if you are going to new it later!!
        m_pGLSLEffectResources[i].ppAttribNames = NULL;
    }
    m_currentGLSLEffectResourceIndex = 0;

    //HLSL shader pool initialisation
    m_HLSLShaderCodes.insert(std::pair<std::string, ID3D10Blob*>("SampleVertexShader", nullptr));
    m_HLSLShaderCodes.insert(std::pair<std::string, ID3D10Blob*>("SamplePixelShader", nullptr));

    //mesh pool initialisation
    m_meshResourceCount = 2;
    m_pMeshResources = new Mesh[m_meshResourceCount];
    for (int i = 0; i < m_meshResourceCount; i++)
    {
        m_pMeshResources[i].pPositions = NULL;
        m_pMeshResources[i].pColours = NULL;
    }
    m_currentMeshResourceIndex = 0;

    //texture2D pool initialisation
    m_texture2DResourceCount = 1;
    m_pTexture2DResources = new Texture2D[m_texture2DResourceCount];
    for (int i = 0; i < m_texture2DResourceCount; i++)
    {
        m_pTexture2DResources[i].pData = NULL;
    }
    m_currentTexture2DResourceIndex = 0;

    m_pAssetIOHandler = new AssetIOHandler;
}

ResourceManager::~ResourceManager()
{
    //release text pool
    for (int i = 0; i < m_textResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_ppTextResources[i]);
    }
    SAFE_DELETEARRAY(m_ppTextResources);

    //release GLSL effect pool
    DestroyGLSLEffectPool();

    //release HLSL shader pool
    DestroyHLSLShaderPool();

    //relese mesh pool
    for (int i = 0; i < m_meshResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_pMeshResources[i].pPositions);
        SAFE_DELETEARRAY(m_pMeshResources[i].pColours);
    }
    SAFE_DELETEARRAY(m_pMeshResources);

    //release texture2D pool
    for (int i = 0; i < m_texture2DResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_pTexture2DResources[i].pData);
    }
    SAFE_DELETEARRAY(m_pTexture2DResources);

    SAFE_DELETE(m_pAssetIOHandler);
}

int ResourceManager::GenerateTextResource(LPCWSTR pFileName, LPCWSTR pType)
{
    if (m_currentTextResourceIndex > m_textResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new text. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    if (m_pAssetIOHandler->LoadAsset(pFileName) == FALSE)
        return -1;
    //copy text to resource pool from asset io handler's local buffer
    if (lstrcmpi(pType, L"ascii") == 0)//in ANSI ASCII, convert to WCHAR
    {
        m_ppTextResources[m_currentTextResourceIndex] = new WCHAR[m_pAssetIOHandler->GetBufferSize() + 1];//one byte is one character
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
            (LPCSTR)m_pAssetIOHandler->GetBuffer(), -1,
            m_ppTextResources[m_currentTextResourceIndex], m_pAssetIOHandler->GetBufferSize());

        //add null terminator for the text string
        m_ppTextResources[m_currentTextResourceIndex][m_pAssetIOHandler->GetBufferSize()] = L'\0';
    }
    else
    {
        //now assume else case is only that the file is encoded in UTF-16 already (need to test this branch in the future)
        m_ppTextResources[m_currentTextResourceIndex] = new WCHAR[m_pAssetIOHandler->GetBufferSize() / sizeof(WCHAR) + 1];//every two bytes are one character
        const BYTE* pRawData = m_pAssetIOHandler->GetBuffer();
        int i, j;
        for (i = 0, j = 0; i < int(m_pAssetIOHandler->GetBufferSize() / sizeof(WCHAR)); i++, j += 2)
        {
            m_ppTextResources[m_currentTextResourceIndex][i] = pRawData[j];
        }

        //add null terminator for the text string
        m_ppTextResources[m_currentTextResourceIndex][m_pAssetIOHandler->GetBufferSize() / sizeof(WCHAR)] = L'\0';
    }
    //clear asset io handler's local buffer
    m_pAssetIOHandler->SetBuffer(NULL);
    m_pAssetIOHandler->SetBufferSize(0);


    m_currentTextResourceIndex++;
    return m_currentTextResourceIndex - 1;//return the index for this text in the pool
}
int ResourceManager::GenerateGLSLEffectResource(GLSLShader* pRequiredShaders, int requiredShaderCount, LPWSTR pName, LPWSTR* ppAttribs, int attribNum)
{
    if (m_currentGLSLEffectResourceIndex > m_GLSLEffectResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new GLSL effect. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].glShaderCount = requiredShaderCount;
    m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].pEffectName = pName;
    m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].pGLSLShaders = new GLSLShader[requiredShaderCount];
    //copy each required shader
    for (int i = 0; i < requiredShaderCount; i++)
    {
        m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].pGLSLShaders[i].type = pRequiredShaders[i].type;
        m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].pGLSLShaders[i].pFileName = pRequiredShaders[i].pFileName;
        m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].pGLSLShaders[i].index = GenerateTextResource(pRequiredShaders[i].pFileName, L"ascii");
        if (m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].pGLSLShaders[i].index == -1)
            return -1;
    }

    //copy attribute names
    m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].ppAttribNames = new LPWSTR[attribNum];
    for (int i = 0; i < attribNum; i++)
    {
        m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].ppAttribNames[i] = ppAttribs[i];//string literals are stored statically so can just copy pointers
    }
    m_pGLSLEffectResources[m_currentGLSLEffectResourceIndex].attribCount = attribNum;

    m_currentGLSLEffectResourceIndex++;
    return m_currentGLSLEffectResourceIndex - 1;//return the inexe for this GLSL effect in the pool
}
HRESULT ResourceManager::GenerateHLSLShaderResource()
{
    ID3D10Blob* pShaderBlob = nullptr;
    ID3D10Blob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"SampleVertexShader.hlsl", NULL, NULL, "main", "vs_5_0",
        0, 0, &pShaderBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(0, (const char*)pErrorBlob->GetBufferPointer(), "VertexShaderError", 0);
        }
        RELEASECOM(pShaderBlob);
        RELEASECOM(pErrorBlob);

        return hr;
    }
    RELEASECOM(m_HLSLShaderCodes["SampleVertexShader"]);
    m_HLSLShaderCodes["SampleVertexShader"] = pShaderBlob;
    RELEASECOM(pErrorBlob);

    hr = D3DCompileFromFile(L"SamplePixelShader.hlsl", NULL, NULL, "main", "ps_5_0",
        0, 0, &pShaderBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(0, (const char*)pErrorBlob->GetBufferPointer(), "PixelShaderError", 0);
        }
        RELEASECOM(pShaderBlob);
        RELEASECOM(pErrorBlob);

        return hr;
    }

    RELEASECOM(m_HLSLShaderCodes["SamplePixelShader"]);
    m_HLSLShaderCodes["SamplePixelShader"] = pShaderBlob;
    RELEASECOM(pErrorBlob);
    
    return hr;
}
int ResourceManager::GenerateMeshResource(Mesh* pMeshData)
{
    if (m_currentMeshResourceIndex > m_meshResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new Mesh. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    //copy the mesh data into the resource pool
    m_pMeshResources[m_currentMeshResourceIndex].vertexCount = pMeshData->vertexCount;
    m_pMeshResources[m_currentMeshResourceIndex].vertexSize = pMeshData->vertexSize;
    m_pMeshResources[m_currentMeshResourceIndex].pPositions = new float[pMeshData->vertexCount * pMeshData->vertexSize];
    for (int i = 0; i < pMeshData->vertexCount * pMeshData->vertexSize; i++)
        m_pMeshResources[m_currentMeshResourceIndex].pPositions[i] = pMeshData->pPositions[i];

    m_pMeshResources[m_currentMeshResourceIndex].pColours = new float[pMeshData->vertexCount * pMeshData->vertexSize];
    for (int i = 0; i < pMeshData->vertexCount * pMeshData->vertexSize; i++)
        m_pMeshResources[m_currentMeshResourceIndex].pColours[i] = pMeshData->pColours[i];

    m_currentMeshResourceIndex++;
    return m_currentMeshResourceIndex - 1;
}
void LoadPNGFromMemory(png_structp png_ptr, png_bytep data, png_size_t length)
{
    //get the buffer head
    png_voidp pSourceBuffer = png_get_io_ptr(png_ptr);

    //read data
    memcpy(data, pSourceBuffer, length);

    //move the buffer head for future read
    png_bytep pNextStart = (png_bytep)pSourceBuffer + length;
    png_set_read_fn(png_ptr, (png_voidp)pNextStart, LoadPNGFromMemory);
}
int ResourceManager::GenerateTexture2DResource(LPCWSTR pFileName, LPCWSTR pType)
{
    if (m_currentTexture2DResourceIndex > m_texture2DResourceCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new Texture. Resource pool limit reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    if (m_pAssetIOHandler->LoadAsset(pFileName) == FALSE)
        return -1;

    //process image data stream based on image type
    if (lstrcmpi(pType, L"jpeg") == 0)
    {
        jpeg_decompress_struct jpegObject;
        jpeg_error_mgr jpegErrorManager;

        //set up the normal JPEG error routines
        jpegObject.err = jpeg_std_error(&jpegErrorManager);
        //jpegErrorManager.error_exit = JpegFatalError;
        //jpegErrorManager.emit_message = JpegErrorMessageEmitter;

        //initialise the JPEG decompression object
        jpeg_create_decompress(&jpegObject);

        //specify raw image data source
        jpeg_mem_src(&jpegObject, (unsigned char*)m_pAssetIOHandler->GetBuffer(), m_pAssetIOHandler->GetBufferSize());

        //parse jpeg header. passed TRUE to reject a tables-only JPEG file 
        jpeg_read_header(&jpegObject, TRUE);

        //start decompressor
        jpeg_start_decompress(&jpegObject);

        //image size info
        int imageWidth = jpegObject.output_width;
        int imageHeight = jpegObject.output_height;
        int channelNum = jpegObject.output_components;
        int row_stride = imageWidth * channelNum;//row width in output buffer in byte (jpeg assumes one channel one byte)

        //allocate temporary space for texture data
        BYTE** rows = new BYTE * [imageHeight];
        for (int i = 0; i < imageHeight; i++)
            rows[i] = new BYTE[row_stride];

        //read rows
        while (jpegObject.output_scanline < jpegObject.output_height) {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
            jpeg_read_scanlines(&jpegObject, &(rows[jpegObject.output_scanline]), 1);
        }

        /* Finish decompression */
        jpeg_finish_decompress(&jpegObject);

        /* Release JPEG decompression object */
        /* This is an important step since it will release a good deal of memory. */
        jpeg_destroy_decompress(&jpegObject);

        //transfer image data from temporary storage to texture2d resource pool
        m_pTexture2DResources[m_currentTexture2DResourceIndex].pData = new BYTE[imageWidth * imageHeight * channelNum];
        for (int i = 0; i < imageHeight; i++)
        {
            memcpy(m_pTexture2DResources[m_currentTexture2DResourceIndex].pData + i * row_stride, rows[i], row_stride);
        }

        /* And we're done! */
        //clear temporary row buffer
        for (int i = 0; i < imageHeight; i++)
        {
            SAFE_DELETEARRAY(rows[i]);
        }
        SAFE_DELETEARRAY(rows);

        //clear asset io handler's local buffer
        m_pAssetIOHandler->SetBuffer(NULL);
        m_pAssetIOHandler->SetBufferSize(0);

        m_currentTexture2DResourceIndex++;
        return m_currentTexture2DResourceIndex - 1;
    }
    else if (lstrcmpi(pType, L"png") == 0)
    {
        //create png_struct
        png_structp pPngstruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (pPngstruct == NULL)
            return -1;

        //create png_info
        png_infop pPnginfo = png_create_info_struct(pPngstruct);
        if (pPnginfo == NULL)
        {
            png_destroy_read_struct(&pPngstruct, NULL, NULL);
            return -1;
        }

        //set the callback function for customised image source read
        png_set_read_fn(pPngstruct, (png_voidp)m_pAssetIOHandler->GetBuffer(), LoadPNGFromMemory);

        //read png info
        png_read_info(pPngstruct, pPnginfo);
        int imageWidth = png_get_image_width(pPngstruct, pPnginfo);
        int imageHeight = png_get_image_height(pPngstruct, pPnginfo);
        int colourType = png_get_color_type(pPngstruct, pPnginfo);
        int bitDepth = png_get_bit_depth(pPngstruct, pPnginfo);
        int row_stride = png_get_rowbytes(pPngstruct, pPnginfo);//row width in output buffer in byte
        int interlace_type = png_get_interlace_type(pPngstruct, pPnginfo);

        //allocate temporary space for texture data
        png_bytep* rows = new png_bytep[imageHeight];
        for (int i = 0; i < imageHeight; i++)
            rows[i] = new png_byte[row_stride];

        //read actual image data
        png_read_image(pPngstruct, rows);

        png_read_end(pPngstruct, NULL);

        png_destroy_read_struct(&pPngstruct, &pPnginfo, NULL);

        //transfer image data from temporary storage to texture2d resource pool
        m_pTexture2DResources[m_currentTexture2DResourceIndex].pData = new BYTE[imageHeight * row_stride];
        for (int i = 0; i < imageHeight; i++)
        {
            memcpy(m_pTexture2DResources[m_currentTexture2DResourceIndex].pData + i * row_stride, rows[i], row_stride);
        }

        /* And we're done! */
        //clear temporary row buffer
        for (int i = 0; i < imageHeight; i++)
        {
            SAFE_DELETEARRAY(rows[i]);
        }
        SAFE_DELETEARRAY(rows);

        //clear asset io handler's local buffer
        m_pAssetIOHandler->SetBuffer(NULL);
        m_pAssetIOHandler->SetBufferSize(0);

        m_currentTexture2DResourceIndex++;
        return m_currentTexture2DResourceIndex - 1;
    }
    else
    {
        //for future extension of more image format supports, but for now
        return -1;
    }
}
ID3D10Blob* ResourceManager::GetHLSLShader(std::string name)
{
    auto targetIterator = m_HLSLShaderCodes.find(name);
    if (targetIterator != m_HLSLShaderCodes.end())
    {
        return targetIterator->second;
    }
    else
    {
        return nullptr;
    }
}
void ResourceManager::DestroyGLSLEffectPool()
{
    for (int i = 0; i < m_GLSLEffectResourceCount; i++)
    {
        SAFE_DELETEARRAY(m_pGLSLEffectResources[i].pGLSLShaders);
        SAFE_DELETEARRAY(m_pGLSLEffectResources[i].ppAttribNames);
    }
    SAFE_DELETEARRAY(m_pGLSLEffectResources);
}
void ResourceManager::DestroyHLSLShaderPool()
{
    for (auto it = m_HLSLShaderCodes.begin(); it != m_HLSLShaderCodes.end(); ++it)
    {
        RELEASECOM(it->second);
    }
    m_HLSLShaderCodes.clear();
}
